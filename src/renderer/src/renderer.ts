function init(): void {
  window.addEventListener('DOMContentLoaded', () => {
    doAThing()
  })
}

function doAThing(): void {
  const versions = window.electron.process.versions
  replaceText('.electron-version', `Electron v${versions.electron}`)
  replaceText('.chrome-version', `Chromium v${versions.chrome}`)
  replaceText('.node-version', `Node v${versions.node}`)

  const ipcHandlerBtnSync = document.getElementById('ipcHandlerSync')
  ipcHandlerBtnSync?.addEventListener('click', async () => {
    try {
      const res = await window.api.onInit()
      console.log('returned', res)
    } catch (e) {
      console.error(e)
    }
  })
  const ipcHandlerBtnAsync = document.getElementById('ipcHandlerAsync')
  ipcHandlerBtnAsync?.addEventListener('click', async () => {
    try {
      const response = await window.api.onUpload('S01E02_The_Kingsroad.mkv')
      console.log(response)
    } catch (e) {
      console.error(e)
    }
  })
}

function replaceText(selector: string, text: string): void {
  const element = document.querySelector<HTMLElement>(selector)
  if (element) {
    element.innerText = text
  }
}

init()
