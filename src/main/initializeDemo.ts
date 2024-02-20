/* eslint-disable prettier/prettier */
import { spawn } from 'node:child_process';
import { ipcMain } from 'electron';

export function initializeDemo(): void {
  const pythonProcess = spawn('python', ['src/server/demo_pc.py']);

  pythonProcess.stdout.on('data', (data) => {
    console.log(`Python stdout: ${data}`);
  });

  pythonProcess.stderr.on('data', (data) => {
    console.error(`Python stderr: ${data}`);
  });

  ipcMain.handle('execute-python', (_event, command, args) => {
    return new Promise((resolve, reject) => {
      try {
        // Assuming jsonString is a stringified JSON of {command: string, args: any[]}
        console.log(`Executing Python function with payload: ${ command, args}`);
        // Send the command to the Python process
        pythonProcess.stdin.write(JSON.stringify({ command, args }) + '\n');

        // Listen for the response
        const listener = (data) => {
          console.log(`Received response from Python: ${data}`);
          pythonProcess.stdout.removeListener('data', listener); // Remove listener to prevent memory leak
          resolve(data.toString().trim());
        };

        pythonProcess.stdout.on('data', listener);
      } catch (error) {
        console.error(`Error communicating with Python script: ${error}`);
        reject(error);
      }
    });
  });
}
