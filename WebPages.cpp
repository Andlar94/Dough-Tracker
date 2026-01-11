#include "WebPages.h"

String WebPages::getIndexHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dough Tracker</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
)rawliteral" + getInlineCSS() + R"rawliteral(
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>🍞 Dough Tracker</h1>
            <div class="wifi-status">
                <span id="wifiStatus" class="status-indicator"></span>
                <span id="ipAddress">IP: --</span>
            </div>
        </header>

        <main>
            <!-- Status Section -->
            <section class="status-section">
                <div class="status-box">
                    <h2 id="initialThickness">0 mm</h2>
                    <p>Initial Thickness</p>
                </div>
                <div class="status-box">
                    <h2 id="thickness">0 mm</h2>
                    <p>Current Thickness</p>
                </div>
                <div class="status-box">
                    <h2 id="risePercentage">0%</h2>
                    <p>Current Rise</p>
                </div>
                <div class="status-box">
                    <h2 id="elapsedTime">0:00</h2>
                    <p>Elapsed Time</p>
                </div>
            </section>

            <!-- Chart Section -->
            <section class="chart-section">
                <h2>Rise Progress</h2>
                <div class="chart-container">
                    <canvas id="riseChart"></canvas>
                </div>
            </section>

            <!-- Controls Section -->
            <section class="controls-section">
                <h2>Controls</h2>
                <div class="button-grid">
                    <button onclick="calibrateZero()" class="btn btn-primary">📏 Calibrate empty container</button>
                    <button onclick="calibrateDough()" class="btn btn-primary">🍞 Calibrate fresh starter</button>
                    <button onclick="forceMeasurement()" class="btn btn-success">📊 Force Measurement</button>
                    <button onclick="showOffsetDialog()" class="btn btn-secondary">⚙️ Set Offset</button>
                    <button onclick="resetData()" class="btn btn-danger">🗑️ Reset Data</button>
                    <button onclick="resetWifi()" class="btn btn-warning">🔌 Reset WiFi</button>
                </div>
            </section>

            <!-- Data Table -->
            <section class="data-section">
                <div class="collapsible-header" onclick="toggleSection('historyContent')">
                    <h2>📊 Measurement History</h2>
                    <span class="toggle-arrow" id="historyArrow">▼</span>
                </div>
                <div id="historyContent" class="collapsible-content">
                    <div class="table-wrapper">
                        <table id="dataTable">
                            <thead>
                                <tr>
                                    <th>Time</th>
                                    <th>Thickness (mm)</th>
                                    <th>Rise (%)</th>
                                </tr>
                            </thead>
                            <tbody id="tableBody">
                            </tbody>
                        </table>
                    </div>
                </div>
            </section>

            <!-- WiFi Configuration Section -->
            <section class="wifi-config-section">
                <div class="collapsible-header" onclick="toggleSection('wifiContent')">
                    <h2>📡 WiFi Configuration</h2>
                    <span class="toggle-arrow" id="wifiArrow">▼</span>
                </div>
                <div id="wifiContent" class="collapsible-content">
                <div class="wifi-controls">
                    <button onclick="scanNetworks()" class="btn btn-primary">Scan Networks</button>
                    <div id="scanStatus" style="margin-top: 10px; display: none;">Scanning...</div>
                </div>
                <div id="networkList" style="margin-top: 15px;">
                    <label for="networkSelect">Available Networks:</label>
                    <select id="networkSelect" style="width: 100%; padding: 8px; margin-top: 8px;">
                        <option value="">-- Select a network --</option>
                    </select>
                </div>
                <div style="margin-top: 15px;">
                    <label for="wifiPassword">Password:</label>
                    <input type="password" id="wifiPassword" placeholder="Enter WiFi password" style="width: 100%; padding: 8px; margin-top: 8px;">
                </div>
                <div style="margin-top: 15px;">
                    <button onclick="connectToWiFi()" class="btn btn-success">Connect</button>
                    <div id="connectStatus" style="margin-top: 10px; display: none; padding: 10px; background: #e3f2fd; border-radius: 6px;"></div>
                </div>
                </div>
            </section>
        </main>

        <footer>
            <p>Sourdough Tracker v1.0 | Last update: <span id="lastUpdate">--:--:--</span></p>
        </footer>
    </div>

    <!-- Offset Dialog -->
    <div id="offsetDialog" class="dialog" style="display: none;">
        <div class="dialog-content">
            <h3>Set Measurement Offset</h3>
            <p>Enter offset value in millimeters:</p>
            <input type="number" id="offsetInput" placeholder="0" min="-50" max="50">
            <div class="dialog-buttons">
                <button onclick="saveOffset()" class="btn btn-primary">Save</button>
                <button onclick="closeDialog()" class="btn btn-secondary">Cancel</button>
            </div>
        </div>
    </div>

    <script>
)rawliteral" + getInlineJavaScript() + R"rawliteral(
    </script>
</body>
</html>
)rawliteral";
}

String WebPages::getInlineCSS() {
  return R"rawliteral(
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    background: linear-gradient(135deg, #1a1a1a 0%, #666666 100%);
    min-height: 100vh;
    color: #333;
}

.container {
    max-width: 1000px;
    margin: 0 auto;
    padding: 20px;
}

header {
    background: white;
    border-radius: 10px;
    padding: 20px;
    margin-bottom: 20px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    display: flex;
    justify-content: space-between;
    align-items: center;
}

header h1 {
    font-size: 28px;
    color: #333;
}

.wifi-status {
    display: flex;
    align-items: center;
    gap: 10px;
    font-size: 14px;
}

.status-indicator {
    width: 12px;
    height: 12px;
    border-radius: 50%;
    background: #ff6b6b;
    display: inline-block;
}

.status-indicator.connected {
    background: #51cf66;
}

main {
    display: grid;
    gap: 20px;
}

section {
    background: white;
    border-radius: 10px;
    padding: 20px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}

section h2 {
    margin-bottom: 15px;
    color: #333;
    font-size: 20px;
}

.status-section {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 15px;
}

.status-box {
    text-align: center;
    padding: 20px;
    background: linear-gradient(135deg, #1a1a1a 0%, #555555 100%);
    color: white;
    border-radius: 8px;
}

.status-box h2 {
    font-size: 32px;
    margin-bottom: 10px;
    color: white;
}

.status-box p {
    font-size: 14px;
    opacity: 0.9;
}

.chart-container {
    position: relative;
    height: 300px;
}

.table-wrapper {
    overflow-x: auto;
}

table {
    width: 100%;
    border-collapse: collapse;
}

thead {
    background: #f5f5f5;
}

th {
    padding: 12px;
    text-align: left;
    font-weight: 600;
    border-bottom: 2px solid #ddd;
}

td {
    padding: 10px 12px;
    border-bottom: 1px solid #eee;
}

tr:hover {
    background: #f9f9f9;
}

.controls-section {
    background: #f9f9f9;
}

.button-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 10px;
}

.btn {
    padding: 12px 16px;
    border: none;
    border-radius: 6px;
    font-size: 14px;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.3s ease;
}

.btn:hover {
    transform: translateY(-2px);
    box-shadow: 0 4px 12px rgba(0,0,0,0.15);
}

.btn-primary {
    background: #667eea;
    color: white;
}

.btn-primary:hover {
    background: #5568d3;
}

.btn-success {
    background: #51cf66;
    color: white;
}

.btn-success:hover {
    background: #40c057;
}

.btn-secondary {
    background: #748094;
    color: white;
}

.btn-secondary:hover {
    background: #636e7b;
}

.btn-danger {
    background: #ff6b6b;
    color: white;
}

.btn-danger:hover {
    background: #ee5a52;
}

.btn-warning {
    background: #ffd93d;
    color: #333;
}

.btn-warning:hover {
    background: #f5c200;
}

footer {
    text-align: center;
    padding: 20px;
    color: white;
    font-size: 12px;
}

.dialog {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0,0,0,0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.dialog-content {
    background: white;
    border-radius: 10px;
    padding: 30px;
    box-shadow: 0 10px 40px rgba(0,0,0,0.3);
    max-width: 400px;
}

.dialog-content h3 {
    margin-bottom: 15px;
    color: #333;
}

.dialog-content input {
    width: 100%;
    padding: 10px;
    margin: 15px 0;
    border: 1px solid #ddd;
    border-radius: 6px;
    font-size: 16px;
}

.dialog-buttons {
    display: flex;
    gap: 10px;
    margin-top: 20px;
}

.dialog-buttons button {
    flex: 1;
}

.collapsible-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    cursor: pointer;
    user-select: none;
    margin: -20px -20px 0 -20px;
    padding: 20px;
    border-bottom: 1px solid #eee;
}

.collapsible-header h2 {
    margin: 0;
}

.toggle-arrow {
    font-size: 12px;
    transition: transform 0.3s ease;
    color: #999999;
}

.collapsible-content {
    display: none;
    animation: slideDown 0.3s ease;
}

.collapsible-content.expanded {
    display: block;
}

@keyframes slideDown {
    from {
        opacity: 0;
        max-height: 0;
    }
    to {
        opacity: 1;
        max-height: 1000px;
    }
}

.wifi-content-expanded {
    display: block;
}

@media (max-width: 600px) {
    header {
        flex-direction: column;
        gap: 15px;
    }

    .button-grid {
        grid-template-columns: 1fr;
    }
}
)rawliteral";
}

String WebPages::getInlineJavaScript() {
  return R"rawliteral(
let chart = null;
let autoRefreshInterval = null;

// Toggle section visibility
function toggleSection(contentId) {
    const content = document.getElementById(contentId);
    const arrow = document.getElementById(contentId === 'historyContent' ? 'historyArrow' : 'wifiArrow');
    
    if (content.classList.contains('expanded')) {
        content.classList.remove('expanded');
        arrow.style.transform = 'rotate(0deg)';
    } else {
        content.classList.add('expanded');
        arrow.style.transform = 'rotate(180deg)';
    }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', function() {
    initializeChart();
    updateStatus();
    autoRefreshInterval = setInterval(updateStatus, 30000); // Update every 30 seconds
});

function initializeChart() {
    const ctx = document.getElementById('riseChart');
    if (!ctx) return;
    
    chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: 'Dough Rise (%)',
                data: [],
                borderColor: '#667eea',
                backgroundColor: 'rgba(102, 126, 234, 0.1)',
                tension: 0.3,
                fill: true,
                pointBackgroundColor: '#667eea',
                pointBorderColor: '#fff',
                pointBorderWidth: 2,
                pointRadius: 5,
                pointHoverRadius: 7
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: true,
                    position: 'top'
                }
            },
            scales: {
                y: {
                    beginAtZero: true,
                    title: {
                        display: true,
                        text: 'Rise Percentage (%)'
                    }
                }
            }
        }
    });
}

function updateStatus() {
    // Fetch data from /data endpoint
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            updateUI(data);
        })
        .catch(error => {
            console.error('Error fetching data:', error);
        });
    
    // Fetch WiFi and calibration status
    fetch('/status')
        .then(response => response.json())
        .then(data => {
            updateWifiStatus(data);
            updateCalibrationStatus(data);
        })
        .catch(error => {
            console.error('Error fetching status:', error);
        });
}

function updateUI(data) {
    if (!data || !data.measurements || data.measurements.length === 0) {
        return;
    }
    
    const latest = data.measurements[data.measurements.length - 1];
    
    // Update status boxes
    document.getElementById('risePercentage').textContent = latest.rise.toFixed(1) + '%';
    document.getElementById('thickness').textContent = latest.thickness + ' mm';
    
    // Calculate elapsed time using Unix timestamps
    const firstTime = data.measurements[0].timestamp * 1000;  // Convert to milliseconds
    const lastTime = latest.timestamp * 1000;  // Convert to milliseconds
    const elapsedSeconds = Math.floor((lastTime - firstTime) / 1000);
    const hours = Math.floor(elapsedSeconds / 3600);
    const minutes = Math.floor((elapsedSeconds % 3600) / 60);
    document.getElementById('elapsedTime').textContent = hours + ':' + String(minutes).padStart(2, '0');
    
    // Update chart
    updateChart(data.measurements);
    
    // Update table
    updateTable(data.measurements);
    
    // Update last update time
    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
}

function formatTime24h(timestamp) {
    // Format timestamp in 24hr format for Bergen, Norway (Europe/Oslo timezone)
    const date = new Date(timestamp * 1000);
    return date.toLocaleString('no-NO', {
        hour12: false,
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        timeZone: 'Europe/Oslo'
    });
}

function updateChart(measurements) {
    if (!chart) return;
    
    // Calculate elapsed time for each measurement
    const firstTimestamp = measurements[0].timestamp;
    chart.data.labels = measurements.map(m => {
        // Get clock time
        const timeStr = formatTime24h(m.timestamp);
        
        // Calculate elapsed time
        const elapsedSeconds = Math.floor((m.timestamp - firstTimestamp));
        const hours = Math.floor(elapsedSeconds / 3600);
        const minutes = Math.floor((elapsedSeconds % 3600) / 60);
        
        // Format as "HH:MM (Elapsed: H:MM)" (e.g., "10:15 (Elapsed: 0:05)")
        return timeStr + '\n(Elapsed: ' + hours + ':' + String(minutes).padStart(2, '0') + ')';
    });
    
    chart.data.datasets[0].data = measurements.map(m => m.rise);
    chart.update();
}

function updateTable(measurements) {
    const tbody = document.getElementById('tableBody');
    tbody.innerHTML = '';
    
    measurements.forEach(m => {
        const row = tbody.insertRow();
        // Use 24hr format for time display
        const timeStr = formatTime24h(m.timestamp);
        row.innerHTML = `
            <td>${timeStr}</td>
            <td>${m.thickness} mm</td>
            <td>${m.rise.toFixed(1)}%</td>
        `;
    });
}

function updateWifiStatus(status) {
    const indicator = document.querySelector('.status-indicator');
    const ipElement = document.getElementById('ipAddress');
    const wifiContent = document.getElementById('wifiContent');
    const wifiArrow = document.getElementById('wifiArrow');
    
    if (status.wifiConnected) {
        indicator.classList.add('connected');
        ipElement.textContent = 'IP: ' + status.ip;
        
        // Auto-collapse WiFi section when connected
        if (wifiContent && wifiContent.classList.contains('expanded')) {
            wifiContent.classList.remove('expanded');
            wifiArrow.style.transform = 'rotate(0deg)';
        }
    } else {
        indicator.classList.remove('connected');
        ipElement.textContent = 'IP: Not connected';
        
        // Auto-expand WiFi section when disconnected
        if (wifiContent && !wifiContent.classList.contains('expanded')) {
            wifiContent.classList.add('expanded');
            wifiArrow.style.transform = 'rotate(180deg)';
        }
    }
}

function updateCalibrationStatus(status) {
    // Update initial thickness from calibration
    const initialThicknessElement = document.getElementById('initialThickness');
    if (initialThicknessElement) {
        initialThicknessElement.textContent = status.initialThickness + ' mm';
    }
}

function calibrateZero() {
    if (!confirm('Put empty container in sensor. Continue?')) return;
    
    fetch('/api/calibrate', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            alert('Calibration set to: ' + data.zeroPoint + ' mm');
            updateStatus();
        })
        .catch(error => alert('Error: ' + error));
}

function calibrateDough() {
    if (!confirm('Put fresh dough in container. Continue?')) return;
    
    fetch('/api/calibrate-dough', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            alert('Dough calibrated!\nInitial thickness: ' + data.initialThickness + ' mm');
            updateStatus();
        })
        .catch(error => alert('Error: ' + error));
}

function forceMeasurement() {
    fetch('/api/measure', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            alert('Measurement taken: ' + data.thickness + ' mm');
            updateStatus();
        })
        .catch(error => alert('Error: ' + error));
}

function showOffsetDialog() {
    document.getElementById('offsetDialog').style.display = 'flex';
}

function closeDialog() {
    document.getElementById('offsetDialog').style.display = 'none';
}

function saveOffset() {
    const offset = parseInt(document.getElementById('offsetInput').value) || 0;
    
    fetch('/api/offset', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ offset: offset })
    })
    .then(response => response.json())
    .then(data => {
        alert('Offset set to: ' + data.offset + ' mm');
        closeDialog();
        updateStatus();
    })
    .catch(error => alert('Error: ' + error));
}

function resetData() {
    if (!confirm('Delete all measurement data? This cannot be undone.')) return;
    
    fetch('/api/reset-data', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            alert('Data reset complete');
            updateStatus();
        })
        .catch(error => alert('Error: ' + error));
}

function resetWifi() {
    if (!confirm('Reset WiFi settings? Device will reboot.')) return;
    
    fetch('/api/reset-wifi', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            alert('WiFi reset - device rebooting...');
        })
        .catch(error => alert('Error: ' + error));
}

function scanNetworks() {
    const scanStatus = document.getElementById('scanStatus');
    scanStatus.style.display = 'block';
    scanStatus.textContent = 'Scanning...';
    
    fetch('/api/scan-networks')
        .then(response => response.json())
        .then(data => {
            const select = document.getElementById('networkSelect');
            select.innerHTML = '<option value="">-- Select a network --</option>';
            
            data.forEach(network => {
                const option = document.createElement('option');
                option.value = network.ssid;
                option.textContent = network.ssid + ' (Signal: ' + network.rssi + ' dBm)';
                select.appendChild(option);
            });
            
            scanStatus.textContent = 'Found ' + data.length + ' networks';
            setTimeout(() => { scanStatus.style.display = 'none'; }, 3000);
        })
        .catch(error => {
            scanStatus.textContent = 'Error scanning networks: ' + error;
            setTimeout(() => { scanStatus.style.display = 'none'; }, 3000);
        });
}

function connectToWiFi() {
    const ssid = document.getElementById('networkSelect').value;
    const password = document.getElementById('wifiPassword').value;
    const connectStatus = document.getElementById('connectStatus');
    
    if (!ssid) {
        alert('Please select a network');
        return;
    }
    
    connectStatus.style.display = 'block';
    connectStatus.textContent = 'Connecting to ' + ssid + '...';
    
    fetch('/api/connect-wifi', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ssid: ssid, password: password })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            connectStatus.textContent = 'Successfully connected to ' + ssid + '! Device IP: ' + data.ip;
            connectStatus.style.backgroundColor = '#c8e6c9';
            setTimeout(() => {
                connectStatus.style.display = 'none';
                // Refresh page to show new connection
                setTimeout(() => { location.reload(); }, 2000);
            }, 3000);
        } else {
            connectStatus.textContent = 'Connection failed: ' + (data.error || 'Unknown error');
            connectStatus.style.backgroundColor = '#ffcdd2';
        }
    })
    .catch(error => {
        connectStatus.textContent = 'Error: ' + error;
        connectStatus.style.backgroundColor = '#ffcdd2';
    });
}
)rawliteral";
}
