function Search-Content {
    param (
        [Parameter(Mandatory=$true)]
        [string]$Pattern,
        
        [Parameter(Mandatory=$true)]
        [string]$FolderPath,
        
        [string[]]$Extensions = @("*.txt", "*.log", "*.csv", "*.docx", "*.pdf"),
        
        [switch]$CaseSensitive
    )

    # Créer l'objet Word COM pour les fichiers docx
    $word = $null
    if ($Extensions -contains "*.docx") {
        try {
            $word = New-Object -ComObject Word.Application
            $word.Visible = $false
        } catch {
            Write-Warning "Microsoft Word n'est pas disponible. Les fichiers DOCX ne seront pas analysés."
        }
    }
    
    # Récupérer les fichiers correspondant aux extensions
    $files = Get-ChildItem -Path $FolderPath -Include $Extensions -Recurse
    
    foreach ($file in $files) {
        try {
            $contenu = $null
            
            switch ($file.Extension.ToLower()) {
                ".txt" { $contenu = Get-Content -Path $file.FullName -Raw }
                ".log" { $contenu = Get-Content -Path $file.FullName -Raw }
                ".csv" { $contenu = Get-Content -Path $file.FullName -Raw }
                ".docx" {
                    if ($word) {
                        $doc = $word.Documents.Open($file.FullName, $false, $true)
                        $contenu = $doc.Content.Text
                        $doc.Close()
                    }
                }
                ".pdf" {
                    # Nécessite un module PDF comme ImportPdf
                    if (Get-Command Import-Pdf -ErrorAction SilentlyContinue) {
                        $contenu = Import-Pdf -Path $file.FullName
                    } else {
                        Write-Warning "Module ImportPdf requis pour les fichiers PDF"
                    }
                }
            }
            
            if ($contenu) {
                $matched = if ($CaseSensitive) {
                    $contenu -cmatch $Pattern
                } else {
                    $contenu -match $Pattern
                }
                
                if ($matched) {
                    [PSCustomObject]@{
                        FilePath = $file.FullName
                        FileName = $file.Name
                        FileType = $file.Extension
                        MatchFound = $true
                    }
                }
            }
        }
        catch {
            Write-Warning "Erreur lors de l'analyse de $($file.FullName): $_"
        }
    }
    
    # Libérer les ressources Word si utilisées
    if ($word) {
        $word.Quit()
        [System.Runtime.Interopservices.Marshal]::ReleaseComObject($word) | Out-Null
    }
}

# Utilisation
$resultats = Search-Content -Pattern "mot_recherche" -FolderPath "C:\chemin\vers\dossier"
$resultats | Format-Table -AutoSize