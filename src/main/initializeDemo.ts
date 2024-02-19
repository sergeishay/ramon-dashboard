import { /*execSync,*/ spawn } from 'node:child_process'
import { ipcMain } from 'electron'
// import { EDemoSync } from '../enums'
// import WebSocket from 'ws'

export function initializeDemo(): void {
  console.log('initialized demo')
  // execSync('python3 src/server/demo_pc.py')
  spawn('python3', ['src/server/demo_pc.py'])
  // const ws = new WebSocket('ws://localhost:65432')

  // ws.on('data', )
  // py.stdout.on('data', console.log)
  ipcMain.handle('init', () => true)
  ipcMain.handle('upload', () => 'testing')
}
