Run this code in the powershell terminal to get ahold of the latex repository files without being stuck as a git submodule
```powershell
# Remove the submodule from the index
git rm --cached report

# Remove the submodule entry from .gitmodules
(Get-Content .\.gitmodules) -notmatch '^\[submodule "report"' | Set-Content .\.gitmodules

# Remove the submodule entry from .git/config
git config --remove-section submodule.report

# Remove the submodule's .git directory
Remove-Item -Path ".git\modules\report" -Recurse -Force

# Delete the .gitmodules file
Remove-Item -Path ".gitmodules" -Force

# Delete .git, .gitattributes, .gitignore, README.md, and the code folder in the report directory
Remove-Item -Path "report\.git" -Recurse -Force
Remove-Item -Path "report\.gitattributes" -Force
Remove-Item -Path "report\.gitignore" -Force
Remove-Item -Path "report\README.md" -Force
Remove-Item -Path "report\code" -Recurse -Force

Write-Host "Submodule 'report' removed successfully, .gitmodules file deleted, and specified files and folders removed from 'report' directory."
```
Note to self: this is how you can update the submodule:
```powershell
git submodule update --init --recursive
cd report
git fetch
git checkout main
git pull
cd ..
git add report
```
