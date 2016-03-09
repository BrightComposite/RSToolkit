Dim fs, args
Set fs = CreateObject("Scripting.FileSystemObject")
Set args = WScript.Arguments
WScript.Echo fs.GetFile(args(0)).DateLastModified