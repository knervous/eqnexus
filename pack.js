import fs from 'node:fs';
import path from 'node:path';
import archiver from 'archiver';
import ini from 'ini';

// Retrieve version and define the zip filename
const version = process.env.VERSION;
const zipFilename = `eqnexus-${version}.zip`;

// Define paths for files and folders
const dinput8Path = path.resolve('Release', 'dinput8.dll');
const coreDllPath = path.resolve('Release', 'core.dll');
const resourcesFolder = path.resolve('resources');

const versionFile = {
    version, 
    filename: zipFilename
}

fs.writeFileSync('version.json', JSON.stringify(versionFile, null, 4));

// Update config.ini
let config = {};
const configPath = path.join(resourcesFolder, 'config.ini');
if (fs.existsSync(configPath)) {
    const fileContent = fs.readFileSync(configPath, 'utf-8');
    config = ini.parse(fileContent);
} else {
    console.log(`config.ini does not exist at ${configPath}. Creating a new one.`);
}

// Ensure the [Version] section exists and update CoreVersion
config.Version = config.Version || {};
config.Version.CoreVersion = version;

// Write the updated config.ini back to the file
fs.writeFileSync(configPath, ini.stringify(config), 'utf-8');
console.log(`Updated config.ini with CoreVersion=${version}`);

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
