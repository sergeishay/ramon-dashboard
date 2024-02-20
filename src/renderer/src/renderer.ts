function init(): void {
  window.addEventListener('DOMContentLoaded', () => {
    setupEventListeners()
  })
}

function setupEventListeners(): void {
  // Assuming window.api.executePython has been adjusted to send {command, args} objects
  document.getElementById('ipcHandlerSync')?.addEventListener('click', async () => {
    try {
      const response = await window.api.executePython({ command: 'get_files_status', args: [] })
      console.log('Files Status:', response)
    } catch (e) {
      console.error('Error getting files status:', e)
    }
  })

  document.getElementById('ipcHandlerAsync')?.addEventListener('click', async () => {
    try {
      const response = await window.api.executePython({
        command: 'upload_file_to_stream',
        args: ['S01E02_The_Kingsroad.mkv']
      })
      console.log('Upload File Response:', response)
    } catch (e) {
      console.error('Error uploading file:', e)
    }
  })

  document.getElementById('get_device_status')?.addEventListener('click', async () => {
    try {
      const response = await window.api.executePython(JSON.stringify({command: 'get_device_status', args: []}));
      console.log('Device Status:', response)
    } catch (e) {
      console.error('Error getting device status:', e)
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
