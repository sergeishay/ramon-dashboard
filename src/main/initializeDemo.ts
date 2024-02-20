/* eslint-disable prettier/prettier */
import { spawn } from 'node:child_process';
import { ipcMain } from 'electron';

export function initializeDemo(): void {
  console.log('Initialized demo');

  ipcMain.handle('execute-python', async (event, functionName, ...args) => {
    console.log(`Executing Python function: ${functionName} with args: ${args}`);
    const pythonArgs = ['src/server/demo_pc.py', functionName, ...args];
    const pythonProcess = spawn('python', pythonArgs);
    console.log(event)
    let data = '';
    for await (const chunk of pythonProcess.stdout) {
      data += chunk;
    }

    let error = '';
    for await (const chunk of pythonProcess.stderr) {
      error += chunk;
    }

    const exitCode = await new Promise((resolve) => {
      pythonProcess.on('close', resolve);
    });

    if (exitCode) {
      throw new Error(`Subprocess terminated with exit code ${exitCode}, stderr: ${error}`);
    }

    return data;
  });

}
