-- Write lyrics of current audio track into file.
-- Simply executing script with osascript did not work with Mac OS X 10.4 
-- (directing output into file via command line shell did not work with right-to-left languages).
-- In case the script reports something like: Expected expression but found unknown token. (-2741),
-- chances are that Xcode messed up the UTF-8 encoding of the file 
-- (we need left and right pointing double angle quotation mark around "class utf8").
-- Best is to edit script file getLyrics.applescript in Scripteditor application of AppleScript. 

try
	tell application "iTunes"
		set lyricsString to lyrics of current track as Unicode text
		set f to (path to temporary items from user domain as text) & "VizKitLyrics.tmp.txt"
		-- ~/Library/Caches/TemporaryItems/
		open for access file f with write permission
		write lyricsString to file f as Çclass utf8È
		close access file f
	end tell
on error errMsg number errNr
	log errNr
	log errMsg
end try