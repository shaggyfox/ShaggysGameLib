function resumeAudio(e) {
  // fix sound output on chrome
  if (typeof Module === 'undefined'
      || typeof Module.SDL2 == 'undefined'
      || typeof Module.SDL2.audioContext == 'undefined')
      return;
  if (Module.SDL2.audioContext.state == 'suspended') {
      Module.SDL2.audioContext.resume();
  }
  if (Module.SDL2.audioContext.state == 'running') {
      document.getElementById('canvas').removeEventListener('click', resumeAudio);
      document.removeEventListener('keydown', resumeAudio);
  }
  // focus window
  window.focus()
}

document.getElementById('canvas').addEventListener('click', resumeAudio);
document.addEventListener('keydown', resumeAudio);
