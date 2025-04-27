// Register the file type - checks for '.tbl' extension
registerFileType((fileExt, filePath, fileData) => {
    // Check for tbl extension
    return (fileExt === 'tbl');
    // Optional: Add checks using fileData if needed, e.g., reading the magic number
    /*
    if (fileExt === 'tbl') {
        try {
            // Check for 'MYTB' magic number (Little Endian: 0x42, 0x54, 0x59, 0x4D)
            const magicBytes = fileData.getBytesAt(0, 4);
            if (magicBytes.length === 4 &&
                magicBytes[0] === 0x4D && // M
                magicBytes[1] === 0x59 && // Y
                magicBytes[2] === 0x54 && // T
                magicBytes[3] === 0x42) { // B
                 return true;
            }
            // Note: The example above reads Big Endian because getBytesAt returns bytes in file order.
            // If the magic number 'MYTB' is stored Little Endian (4D 59 54 42 becomes 42 54 59 4D in the file),
            // the check would be:
            // if (magicBytes.length === 4 &&
            //     magicBytes[0] === 0x42 && // B
            //     magicBytes[1] === 0x54 && // T
            //     magicBytes[2] === 0x59 && // Y
            //     magicBytes[3] === 0x4D) { // M
            //      return true;
            // }
        } catch (e) {
            // Error reading bytes (e.g., file too small)
            return false;
        }
    }
    return false;
    */
});

// Register the parser function
registerParser(() => {
    // Note: No need for addStandardHeader(); the viewer adds the header automatically.
    // Note: No need to check data type; the viewer framework handles providing the data accessors.
    // Default endianness is Little Endian, matching your original DataView usage.
    // Use setEndianness('BE') if Big Endian is needed.

    // Read the magic number (4 bytes)
    read(4);
    // Use getHex0xValue for magic numbers, often represented in hex.
    // 'MYTB' ASCII: M=4D, Y=59, T=54, B=42. Little Endian stores it as 42 54 59 4D.
    addRow('Magic', getHex0xValue(), 'Expected: 0x4D595442 (MYTB in Little Endian)');

    // Read the version (1 byte)
    read(1);
    addRow('Version', getNumberValue(), 'Version number');

    // Read page size (4 bytes, uint32 LE)
    read(4);
    // Use getDecimalValue() for potentially large unsigned integers to avoid JS precision issues
    addRow('Page Size', getDecimalValue());

    // Read number of records (4 bytes, uint32 LE)
    read(4);
    addRow('Number of Records', getDecimalValue());

    // Read schema offset (4 bytes, uint32 LE)
    read(4);
    const schemaOffset = getNumberValue(); // Get the value to calculate the jump
    addRow('Schema Offset', schemaOffset, 'Absolute location of the schema');

    // --- Jump to Schema Offset ---
    // Calculate how many bytes are between the current position and the schema offset.
    // Current position = 4 (magic) + 1 (ver) + 4 (pgSize) + 4 (numRec) + 4 (schemaOff) = 17 bytes
    const currentOffset = 17;
    const bytesToSkip = schemaOffset - currentOffset;

    if (bytesToSkip < 0) {
        // This indicates an invalid schema offset (points before the current position)
        addRow('Error', 'Invalid Schema Offset', `Offset ${schemaOffset} points before end of header (${currentOffset})`);
        // Optional: Stop parsing here if needed, or attempt to continue if appropriate.
        // dbgStop(); // Uncomment to stop parsing on error
        return; // Stop the parser function
    } else if (bytesToSkip > 0) {
        // Read and discard the bytes between the header and the schema
        read(bytesToSkip);
        addRow('Data Region', `Skipped ${bytesToSkip} bytes`, 'Region between header and schema');
    }
    // Now the internal read pointer is at the schemaOffset

    // --- Read Schema Data ---

    // Read the number of columns (4 bytes, uint32 LE)
    
    read(4);
    const numColumns = getNumberValue();
    addRow('Number of Columns', numColumns, 'Columns in the schema');

    const columns = [];
    const typeCounts = { 1: 0, 2: 0, 3: 0 }; // INT = 1, TEXT = 2, FLOAT = 3

    // Use readRowWithDetails for the schema section to group columns and calculate total size
    readRowWithDetails('Schema Definition', () => {
        for (let i = 0; i < numColumns; i++) {
            // Read the column name length (1 byte)
            read(1);
            const columnNameLength = getNumberValue();
            // Add a row for the length itself (optional, but good for debugging)
            // addRow(`Column ${i + 1} Name Length`, columnNameLength);

            // Read the column name (variable length string)
            read(columnNameLength);
            const columnName = getStringValue();

            // Read the column type (1 byte)
            read(1);
            const columnType = getNumberValue();

            columns.push({ columnName, columnType });

            // Add a row *inside* the details for each column
            addRow(`Column ${i + 1}`, columnName, `Type: ${columnType}`);

            // Update type counts for the chart
            if (typeCounts[columnType] !== undefined) {
                typeCounts[columnType]++;
            } else {
                // Handle potential unknown types if necessary
                addRow('Warning', `Unknown column type ${columnType} for column ${columnName}`);
            }
        }
        // Return value and description for the main 'Schema Definition' row
        return {
            value: `${numColumns} columns`,
            description: 'Details of each column name and type'
        };
    }, true); // true = start expanded

    // --- Add Chart ---
    // Create a chart to visualize the schema types
    if (numColumns > 0) {
        addChart({
            type: 'pie', // Pie chart to visualize column type distribution
            series: [
                // Use the {name, y} format for pie charts
                { name: 'INT (1)', y: typeCounts[1] },
                { name: 'TEXT (2)', y: typeCounts[2] },
                { name: 'FLOAT (3)', y: typeCounts[3] }
            ],
            name: 'Column Types Distribution', // Chart Title
        });
    }

    // --- Handle Remaining Data / Padding ---
    // If you know there's specific padding, read it.
    // Otherwise, you can read the rest of the file if needed.
    // The parser will automatically stop at the end of the file if you don't read everything.
    // Example: Read all remaining bytes and show as 'Remaining Data'
    read(); // Read all remaining bytes
    if (getCurrentReadSize() > 0) { // Check if any bytes were actually read
         addRow('Remaining Data', `Skipped ${getCurrentReadSize()} bytes`, 'Data after the schema');
    }

}); // End of registerParser