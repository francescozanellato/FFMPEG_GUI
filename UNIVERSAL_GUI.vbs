Dim WshShell, exePath, cmd, i, scriptDir

Set WshShell = CreateObject("WScript.Shell")
' Set the script folder as working folder
scriptDir = Left(WScript.ScriptFullName, InStrRev(WScript.ScriptFullName, "\"))
WshShell.CurrentDirectory = scriptDir

' Environment settings
With WshShell.Environment("PROCESS")
    .Item("PATH") = "C:\msys64\mingw64\bin\;.\;.\bin\"
    .Item("QT_PLUGIN_PATH") = ".\share\qt6\plugins;C:\msys64\mingw64\share\qt6\plugins\"
End With

exePath = scriptDir & Replace(WScript.ScriptName, ".vbs", ".exe")

' Inizializza il comando con l'eseguibile
cmd = chr(34) & exePath & chr(34)

' If a fixed parameter is needed before the file, uncomment the following line:
' cmd = cmd & " --modalita-importazione" 

' Ad the files dropped on the script
For i = 0 To WScript.Arguments.Count - 1
    cmd = cmd & " """ & WScript.Arguments(i) & """"
Next

' Run
WshShell.Run cmd, 1

Set WshShell = Nothing
