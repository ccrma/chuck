var chuckEditor = ace.edit("program");
chuckEditor.setTheme("ace/theme/chuck");
chuckEditor.session.setMode("ace/mode/chuck");
chuckEditor.setOptions({
    fontSize: "14px",
    fontFamily: "Monaco",
    cursorStyle: "ace",
    useSoftTabs: true,
    showFoldWidgets: true,
    foldStyle: "markbeginend",
    maxLines: 23,
    minLines: 23
}); 
chuckEditor.container.style.lineHeight = 1.25;
chuckEditor.renderer.updateFontSize();
chuckEditor.session.setUseWrapMode(true);
