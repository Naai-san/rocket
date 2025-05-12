param(
    [Parameter(Mandatory=$true)]
    [string]$DossierDeRecherche,

    [Parameter(Mandatory=$true)]
    [string]$MotAChercher
)

Write-Host "Recherche du mot '$MotAChercher' dans le dossier '$DossierDeRecherche'..."

# --- Recherche dans les fichiers Texte ---
Write-Host "`n--- Recherche dans les fichiers Texte (.txt, .log, .csv, .xml, .json) ---"
$fichiersTexteTrouves = Get-ChildItem -Path $DossierDeRecherche -Recurse -Include "*.txt", "*.log", "*.csv", "*.xml", "*.json" | Select-String -Pattern $MotAChercher -ErrorAction SilentlyContinue
if ($fichiersTexteTrouves) {
    $fichiersTexteTrouves | Select-Object Path, LineNumber, Line
} else {
    Write-Host "Aucun fichier texte contenant le mot n'a été trouvé."
}

# --- Recherche dans les fichiers Word (.docx) ---
Write-Host "`n--- Recherche dans les fichiers Word (.docx) ---"
$wordApp = $null
try {
    $wordApp = New-Object -ComObject Word.Application -ErrorAction Stop
    $wordApp.Visible = $false

    $fichiersDocx = Get-ChildItem -Path $DossierDeRecherche -Recurse -Filter "*.docx"
    if ($fichiersDocx.Count -eq 0) {
        Write-Host "Aucun fichier .docx trouvé dans le dossier."
    } else {
        $docxFonctionne = $true
        $fichiersDocxTrouves = @()
        foreach ($fichierDocx in $fichiersDocx) {
            $document = $null
            try {
                $document = $wordApp.Documents.Open($fichierDocx.FullName, $false, $true) # ReadOnly
                if ($document.Content.Find.Execute($MotAChercher)) {
                    Write-Host "Mot '$MotAChercher' trouvé dans : $($fichierDocx.FullName)"
                    $fichiersDocxTrouves += $fichierDocx.FullName
                }
            }
            catch {
                Write-Warning "Erreur lors du traitement du fichier Word $($fichierDocx.FullName): $($_.Exception.Message)"
            }
            finally {
                if ($document -ne $null) {
                    $document.Close([Microsoft.Office.Interop.Word.WdSaveOptions]::wdDoNotSaveChanges)
                    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($document) | Out-Null
                }
            }
        }
        if ($fichiersDocxTrouves.Count -eq 0 -and $docxFonctionne) {
             Write-Host "Aucun fichier .docx contenant le mot n'a été trouvé."
        }
    }
}
catch {
    Write-Warning "Microsoft Word n'est pas installé ou ne peut pas être démarré. La recherche dans les fichiers .docx est ignorée. Erreur: $($_.Exception.Message)"
    $docxFonctionne = $false
}
finally {
    if ($wordApp -ne $null) {
        $wordApp.Quit()
        [System.Runtime.InteropServices.Marshal]::ReleaseComObject($wordApp) | Out-Null
        Remove-Variable wordApp
    }
}

# --- Placeholder pour la recherche PDF ---
Write-Host "`n--- Recherche dans les fichiers PDF (.pdf) ---"
Write-Host "Pour rechercher dans les PDF, vous devez intégrer un module PowerShell (comme PdfPig)"
Write-Host "ou un outil externe (comme pdftotext.exe de Xpdf)."
Write-Host "Exemple : # Install-Module PdfPig -Scope CurrentUser"
Write-Host "           # Get-ChildItem -Path '$DossierDeRecherche' -Recurse -Filter '*.pdf' | ForEach-Object { ... appeler Get-PdfText ... }"

Write-Host "`nRecherche terminée."