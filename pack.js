import fs from 'node:fs';
import path from 'node:path';
import archiver from 'archiver';

// Retrieve version and define the zip filename
const version = process.env.VERSION;
const zipFilename = `eqnexus-${version}.zip`;

// Define paths for files and folders
const dinput8Path = path.resolve('Release', 'dinput8.dll');
const coreDllPath = path.resolve('Release', 'core.dll');
const resourcesFolder = path.resolve('resources');

// Define output structure for the zip file
const output = fs.createWriteStream(zipFilename);
const archive = archiver('zip', { zlib: { level: 9 } });

output.on('close', () => {
    console.log(`Created zip file ${zipFilename} (${archive.pointer()} total bytes)`);
});
archive.on('error', (err) => {
    throw err;
});

archive.pipe(output);

// Add the root-level dinput8.dll
archive.file(dinput8Path, { name: 'dinput8.dll' });

// Add core.dll to eqnexus folder
archive.file(coreDllPath, { name: 'eqnexus/core.dll' });

// Add contents of the resources folder to eqnexus/resources
fs.readdirSync(resourcesFolder).forEach((file) => {
    const fullPath = path.join(resourcesFolder, file);
    const archivePath = `eqnexus/resources/${file}`;
    if (fs.statSync(fullPath).isDirectory()) {
        archive.directory(fullPath, archivePath);
    } else {
        archive.file(fullPath, { name: archivePath });
    }
});

// Finalize the archive
archive.finalize();
