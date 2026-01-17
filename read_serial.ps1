$port = new-Object System.IO.Ports.SerialPort 'COM7',115200,None,8,one
$port.Open()
Start-Sleep -Seconds 3
$output = ''
for($i=0; $i -lt 30; $i++){
    if($port.BytesToRead -gt 0){
        $output += $port.ReadExisting()
    }
    Start-Sleep -Milliseconds 100
}
$port.Close()
Write-Output $output
