import { contextBridge, ipcRenderer } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'

// Custom APIs for renderer
export const api = {
  onInit: (): Promise<boolean> => ipcRenderer.invoke('init'),
  onUpload: (fileName: string): Promise<unknown> => ipcRenderer.invoke('upload', fileName),
  executePython: (functionName, ...args): Promise<unknown> =>
    ipcRenderer.invoke('execute-python', functionName, ...args)
}

// Use `contextBridge` APIs to expose Electron APIs to
// renderer only if context isolation is enabled, otherwise
// just add to the DOM global.
if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', api)
    // contextBridge.exposeInMainWorld('executePython', executePython)
  } catch (error) {
    console.error(error)
  }
} else {
  // @ts-ignore (define in dts)
  window.electron = electronAPI
  // @ts-ignore (define in dts)
  window.api = api
}
