/* eslint-disable prettier/prettier */
import { /*execSync,*/ spawn } from 'node:child_process'
import { ipcMain } from 'electron'
// import { EDemoSync } from '../enums'
// import WebSocket from 'ws'

export function initializeDemo(): void {
  console.log('initialized demo')
  // execSync('python3 src/server/demo_pc.py')
  // spawn('python3', ['src/server/demo_pc.py'])
  // const ws = new WebSocket('ws://localhost:65432')
  ipcMain.handle('init', async (event) => {
    console.log('ipcMain init ' + event)
    const pythonProcess = spawn('python', ['src/server/demo_pc.py', 'get_device_status'])
    let data = ''
    for await (const chunk of pythonProcess.stdout) {
      data += chunk
    }
    let error = ''
    for await (const chunk of pythonProcess.stderr) {
      error += chunk
    }
    const exitCode = await new Promise((resolve) => {
      pythonProcess.on('close', resolve)
    })
    if (exitCode) {
      throw new Error(`Subprocess terminated with exit code ${exitCode}, stderr: ${error}`)
    }
    // console.log('data', data)
    // console.log('error', error)
    return data
  })
  // ws.on('data', )
  // py.stdout.on('data', console.log)
  // ipcMain.handle('init', () => true)
  ipcMain.handle('upload', () => 'testing')
}
