function Search-ContentInFiles {
    param (
        [Parameter(Mandatory=$true)]
        [string]$FolderPath,
        
        [Parameter(Mandatory=$true)]
        [string]$SearchTerm,
        
        [switch]$Recurse
    )
    
    # Recherche dans fichiers texte (txt, csv, log, etc.)
    Write-Host "Recherche dans les fichiers texte..." -ForegroundColor Cyan
    $textFiles = Get-ChildItem -Path $FolderPath -Include *.txt,*.csv,*.log,*.xml,*.json,*.html,*.htm -Recurse:$Recurse
    $textFiles | ForEach-Object {
        $match = Select-String -Path $_.FullName -Pattern $SearchTerm -ErrorAction SilentlyContinue
        if ($match) {
            [PSCustomObject]@{
                Type = "Texte"
                File = $_.FullName
                MatchCount = $match.Count
            }
        }
    }
    
    # Recherche dans fichiers Word si Word est disponible
    try {
        $word = New-Object -ComObject Word.Application -ErrorAction Stop
        $word.Visible = $false
        
        Write-Host "Recherche dans les fichiers Word..." -ForegroundColor Cyan
        $wordFiles = Get-ChildItem -Path $FolderPath -Include *.docx,*.doc -Recurse:$Recurse
        $wordFiles | ForEach-Object {
            try {
                $doc = $word.Documents.Open($_.FullName, $false, $true)
                if ($doc.Content.Text -match $SearchTerm) {
                    [PSCustomObject]@{
                        Type = "Word"
                        File = $_.FullName
                        MatchCount = "N/A"
                    }
                }
                $doc.Close()
            } catch {
                Write-Warning "Erreur lors de l'ouverture de $($_.FullName): $_"
            }
        }
        $word.Quit()
        [System.Runtime.InteropServices.Marshal]::ReleaseComObject($word) | Out-Null
    } catch {
        Write-Warning "Microsoft Word n'est pas disponible. Les fichiers Word seront ignorés."
    }
    
    # Vous pouvez ajouter d'autres sections pour Excel, PowerPoint, PDF, etc.
}

# Utilisation
Search-ContentInFiles -FolderPath "C:\CheminVersVotreDossier" -SearchTerm "motRecherché" -Recurse | Format-Table -AutoSize