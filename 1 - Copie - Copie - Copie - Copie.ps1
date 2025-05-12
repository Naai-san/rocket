$root   = "C:\Chemin\Vers\VotreDossier"
$motif  = "votreMot"

# 1) txt, log, …
$extText = @("*.txt","*.log","*.csv","*.html","*.xml")
$txtHits = Get-ChildItem -Path $root -Recurse -Include $extText -File |
  Select-String -Pattern $motif |
  Select-Object @{n='File';e={$_.Path}},LineNumber,Line

# 2) docx
$docxHits = Get-ChildItem -Path $root -Recurse -Include *.docx -File |
  ForEach-Object { Search-DocxContent $_.FullName $motif }

# 3) pdf
$pdfHits  = Get-ChildItem -Path $root -Recurse -Include *.pdf -File |
  ForEach-Object { Search-PdfContent $_.FullName $motif }

# Affichage consolidé
$txtHits + $docxHits + $pdfHits |
  Format-Table File, InnerFile, LineNumber, Line -AutoSize