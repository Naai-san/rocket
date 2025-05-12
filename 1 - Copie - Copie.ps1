Install-Module -Name PdfToText -Scope CurrentUser

# Importer le module
Import-Module PdfToText

# Chemin du répertoire à analyser
$DirectoryPath = "C:\Chemin\Vers\Repertoire"

# Mot à rechercher
$SearchWord = "votreMot"

# Rechercher dans les fichiers texte
Get-ChildItem -Path $DirectoryPath -Recurse -Filter *.txt | Select-String -Pattern $SearchWord | ForEach-Object {
    Write-Output "Mot trouvé dans fichier texte : $($_.Path)"
}

# Rechercher dans les fichiers Word
Get-ChildItem -Path $DirectoryPath -Recurse -Filter *.docx | ForEach-Object {
    $Word = New-Object -ComObject Word.Application
    $Document = $Word.Documents.Open($_.FullName, $false, $true)

    if ($Document.Content.Text -like "*$SearchWord*") {
        Write-Output "Mot trouvé dans fichier Word : $($_.FullName)"
    }

    $Document.Close()
    $Word.Quit()
}

# Rechercher dans les fichiers PDF
Get-ChildItem -Path $DirectoryPath -Recurse -Filter *.pdf | ForEach-Object {
    $Text = Get-PdfText -Path $_.FullName
    if ($Text -like "*$SearchWord*") {
        Write-Output "Mot trouvé dans fichier PDF : $($_.FullName)"
    }
}