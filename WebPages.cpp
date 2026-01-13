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
    <script src="https://cdn.jsdelivr.net/npm/hammerjs@2.0.8"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-zoom@2.0.1/dist/chartjs-plugin-zoom.min.js"></script>
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
                <p style="font-size: 12px; color: #7D6658; margin-bottom: 10px;">Scroll to zoom, Ctrl+drag to pan, double-click to reset</p>
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
                    <div style="margin-top: 15px;">
                        <button onclick="showResetDataDialog()" class="btn btn-danger">🗑️ Reset Data</button>
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
                <div style="margin-top: 15px; padding-top: 15px; border-top: 1px solid #E6D5C3;">
                    <button onclick="showResetWifiDialog()" class="btn btn-warning">🔌 Reset WiFi</button>
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

    <!-- Confirmation Dialog -->
    <div id="confirmDialog" class="dialog" style="display: none;">
        <div class="dialog-content">
            <h3 id="confirmTitle">Confirm Action</h3>
            <p id="confirmMessage">Are you sure?</p>
            <div class="dialog-buttons">
                <button id="confirmYes" class="btn btn-primary">Confirm</button>
                <button onclick="closeConfirmDialog()" class="btn btn-secondary">Cancel</button>
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
    font-family: Georgia, Palatino, serif;
    background: linear-gradient(135deg, #FAF3E0 0%, #EBD9C5 100%);
    min-height: 100vh;
    color: #3E2723;
}

.container {
    max-width: 1000px;
    margin: 0 auto;
    padding: 20px;
}

header {
    background: white;
    border-radius: 14px;
    padding: 20px;
    margin-bottom: 20px;
    box-shadow: 0 4px 12px rgba(139, 94, 60, 0.15);
    display: flex;
    justify-content: space-between;
    align-items: center;
}

header h1 {
    font-size: 28px;
    color: #3E2723;
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
    background: #FFFAF5;
    border-radius: 14px;
    padding: 20px;
    box-shadow: 0 4px 12px rgba(139, 94, 60, 0.12);
}

section h2 {
    margin-bottom: 15px;
    color: #3E2723;
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
    background: linear-gradient(135deg, #E8956B 0%, #D4A574 100%);
    color: #FAF3E0;
    border-radius: 14px;
    transition: all 0.3s cubic-bezier(0.25, 0.46, 0.45, 0.94);
}

.status-box:hover {
    transform: translateY(-4px) scale(1.03);
    box-shadow: 0 6px 16px rgba(198, 110, 78, 0.25);
}

.status-box h2 {
    font-size: 32px;
    margin-bottom: 10px;
    color: #FAF3E0;
    transition: all 0.5s cubic-bezier(0.25, 0.46, 0.45, 0.94);
}

.status-box p {
    font-size: 14px;
    opacity: 0.95;
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
    background: #F5EFE6;
}

th {
    padding: 12px;
    text-align: left;
    font-weight: 600;
    border-bottom: 2px solid #E6D5C3;
    color: #3E2723;
}

td {
    padding: 10px 12px;
    border-bottom: 1px solid #F0E8DC;
    color: #5D4037;
}

tr:hover {
    background: #FAF3E0;
}

.controls-section {
    background: #F5EFE6;
}

.button-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 10px;
}

.btn {
    padding: 12px 16px;
    border: none;
    border-radius: 10px;
    font-size: 14px;
    font-weight: 600;
    cursor: pointer;
    font-family: 'Trebuchet MS', Tahoma, sans-serif;
    transition: all 0.3s cubic-bezier(0.25, 0.46, 0.45, 0.94);
}

.btn:hover {
    transform: translateY(-3px) scale(1.02);
    box-shadow: 0 6px 16px rgba(198, 110, 78, 0.20);
}

.btn:active {
    transform: translateY(1px) scale(0.98);
    box-shadow: inset 0 2px 4px rgba(139, 94, 60, 0.2);
}

.btn-primary {
    background: #C66E4E;
    color: white;
}

.btn-primary:hover {
    background: #B45F43;
}

.btn-success {
    background: #8B9D6F;
    color: white;
}

.btn-success:hover {
    background: #7A8B60;
}

.btn-secondary {
    background: #9C8275;
    color: white;
}

.btn-secondary:hover {
    background: #8A7366;
}

.btn-danger {
    background: #BA4727;
    color: white;
}

.btn-danger:hover {
    background: #A33C1F;
}

.btn-warning {
    background: #E6B87D;
    color: #3E2723;
}

.btn-warning:hover {
    background: #D4A574;
}

footer {
    text-align: center;
    padding: 20px;
    color: #7D6658;
    font-size: 12px;
}

.dialog {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(139, 94, 60, 0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.dialog-content {
    background: #FFFAF5;
    border-radius: 14px;
    padding: 30px;
    box-shadow: 0 10px 40px rgba(139, 94, 60, 0.3);
    max-width: 400px;
}

.dialog-content h3 {
    margin-bottom: 15px;
    color: #3E2723;
}

.dialog-content input {
    width: 100%;
    padding: 10px;
    margin: 15px 0;
    border: 2px solid #E6D5C3;
    border-radius: 8px;
    font-size: 16px;
    font-family: Georgia, Palatino, serif;
    background: white;
    color: #3E2723;
}

.dialog-content input:focus {
    outline: none;
    border-color: #C66E4E;
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

/* Toast Notification System */
.toast-container {
    position: fixed;
    top: 20px;
    right: 20px;
    z-index: 9999;
    display: flex;
    flex-direction: column;
    gap: 10px;
}

.toast {
    background: white;
    border-left: 4px solid #C66E4E;
    border-radius: 10px;
    padding: 16px 20px;
    box-shadow: 0 4px 12px rgba(139, 94, 60, 0.25);
    min-width: 300px;
    max-width: 400px;
    display: flex;
    align-items: center;
    gap: 12px;
    animation: slideInRight 0.3s ease;
    font-family: 'Trebuchet MS', Tahoma, sans-serif;
}

.toast.success { border-left-color: #8B9D6F; }
.toast.error { border-left-color: #BA4727; }
.toast.warning { border-left-color: #E6B87D; }

.toast-icon {
    font-size: 20px;
    flex-shrink: 0;
}

.toast-message {
    flex: 1;
    color: #3E2723;
    font-size: 14px;
}

@keyframes slideInRight {
    from {
        transform: translateX(400px);
        opacity: 0;
    }
    to {
        transform: translateX(0);
        opacity: 1;
    }
}

@keyframes fadeOut {
    to {
        opacity: 0;
        transform: translateX(400px);
    }
}

/* Button Loading States */
.btn.loading {
    position: relative;
    pointer-events: none;
    opacity: 0.7;
}

.btn.loading::after {
    content: '';
    position: absolute;
    width: 16px;
    height: 16px;
    top: 50%;
    left: 50%;
    margin-left: -8px;
    margin-top: -8px;
    border: 2px solid white;
    border-radius: 50%;
    border-top-color: transparent;
    animation: spin 0.6s linear infinite;
}

@keyframes spin {
    to { transform: rotate(360deg); }
}

/* Status Box Pulse Animation */
.status-box.updated {
    animation: pulse-warm 0.6s ease;
}

@keyframes pulse-warm {
    0%, 100% {
        transform: scale(1);
        box-shadow: 0 4px 8px rgba(139, 94, 60, 0.15);
    }
    50% {
        transform: scale(1.05);
        box-shadow: 0 6px 20px rgba(198, 110, 78, 0.3);
    }
}

@media (max-width: 600px) {
    header {
        flex-direction: column;
        gap: 15px;
    }

    .button-grid {
        grid-template-columns: 1fr;
    }

    .toast {
        min-width: 250px;
        max-width: calc(100vw - 40px);
    }
}
)rawliteral";
}

String WebPages::getInlineJavaScript() {
  return R"rawliteral(
let chart = null;
let autoRefreshInterval = null;
let calibrationTime = 0;  // Store calibration timestamp globally

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
                borderColor: '#C66E4E',
                backgroundColor: 'rgba(198, 110, 78, 0.1)',
                tension: 0.3,
                fill: true,
                pointBackgroundColor: '#C66E4E',
                pointBorderColor: '#FAF3E0',
                pointBorderWidth: 2,
                pointRadius: 2.5,
                pointHoverRadius: 4
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                legend: {
                    display: true,
                    position: 'top'
                },
                zoom: {
                    pan: {
                        enabled: true,
                        mode: 'x',
                        modifierKey: 'ctrl'
                    },
                    zoom: {
                        wheel: {
                            enabled: true,
                            speed: 0.1
                        },
                        pinch: {
                            enabled: true
                        },
                        mode: 'x'
                    },
                    limits: {
                        x: {min: 'original', max: 'original'}
                    }
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

    // Add double-click to reset zoom
    ctx.ondblclick = function() {
        chart.resetZoom();
    };
}

// Toast notification system
function showToast(message, type = 'info') {
    let container = document.querySelector('.toast-container');
    if (!container) {
        container = document.createElement('div');
        container.className = 'toast-container';
        document.body.appendChild(container);
    }

    const icons = {
        success: '✓',
        error: '✗',
        warning: '⚠',
        info: 'ℹ'
    };

    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    toast.innerHTML = `
        <span class="toast-icon">${icons[type] || icons.info}</span>
        <span class="toast-message">${message}</span>
    `;

    container.appendChild(toast);

    setTimeout(() => {
        toast.style.animation = 'fadeOut 0.3s ease forwards';
        setTimeout(() => toast.remove(), 300);
    }, 3000);
}

// Button loading state helpers
function setButtonLoading(button, loading) {
    if (loading) {
        button.classList.add('loading');
        button.disabled = true;
    } else {
        button.classList.remove('loading');
        button.disabled = false;
    }
}

// Custom confirmation dialog
function showConfirmDialog(title, message, onConfirm) {
    document.getElementById('confirmTitle').textContent = title;
    document.getElementById('confirmMessage').textContent = message;
    document.getElementById('confirmDialog').style.display = 'flex';

    // Remove old listeners and add new one
    const yesBtn = document.getElementById('confirmYes');
    const newYesBtn = yesBtn.cloneNode(true);
    yesBtn.parentNode.replaceChild(newYesBtn, yesBtn);

    newYesBtn.onclick = function() {
        closeConfirmDialog();
        onConfirm();
    };
}

function closeConfirmDialog() {
    document.getElementById('confirmDialog').style.display = 'none';
}

function updateStatus() {
    // Fetch data from /data endpoint
    let measurements = null;
    let statusData = null;
    let dataFetched = false;
    let statusFetched = false;
    
    function updateChartIfReady() {
        if (dataFetched && statusFetched && measurements) {
            updateChart(measurements);
        }
    }
    
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            measurements = data.measurements;
            dataFetched = true;
            updateUI(data);
            updateChartIfReady();
        })
        .catch(error => {
            console.error('Error fetching data:', error);
        });
    
    // Fetch WiFi and calibration status
    fetch('/status')
        .then(response => response.json())
        .then(data => {
            statusData = data;
            statusFetched = true;
            updateWifiStatus(data);
            updateCalibrationStatus(data);
            updateChartIfReady();
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

    // Update with pulse animation
    const statusBoxes = document.querySelectorAll('.status-box');
    statusBoxes.forEach(box => box.classList.add('updated'));
    setTimeout(() => {
        statusBoxes.forEach(box => box.classList.remove('updated'));
    }, 600);

    // Update status boxes
    document.getElementById('risePercentage').textContent = latest.rise.toFixed(1) + '%';
    document.getElementById('thickness').textContent = latest.thickness + ' mm';

    // Elapsed time will be updated in updateCalibrationStatus with calibration timestamp
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
    
    // Calculate elapsed time for each measurement from calibration time
    const baselineTime = calibrationTime > 0 ? calibrationTime : measurements[0].timestamp;
    chart.data.labels = measurements.map(m => {
        // Get clock time
        const timeStr = formatTime24h(m.timestamp);
        
        // Calculate elapsed time from baseline (calibration time or first measurement)
        const elapsedSeconds = Math.floor((m.timestamp - baselineTime));
        const hours = Math.floor(elapsedSeconds / 3600);
        const minutes = Math.floor((elapsedSeconds % 3600) / 60);
        
        // Format as "HH:MM (Elapsed: H:MM)" (e.g., "10:15 (Elapsed: 15:00)")
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
    
    // Calculate and update elapsed time from calibration time
    if (status.calibrationTime && status.calibrationTime > 0) {
        // Set the global calibration time so chart can use it
        calibrationTime = status.calibrationTime;
        
        const calibrationTimeSeconds = status.calibrationTime;
        const now = Math.floor(Date.now() / 1000);
        const elapsedSeconds = now - calibrationTimeSeconds;
        
        const hours = Math.floor(elapsedSeconds / 3600);
        const minutes = Math.floor((elapsedSeconds % 3600) / 60);
        
        const elapsedTimeElement = document.getElementById('elapsedTime');
        if (elapsedTimeElement) {
            elapsedTimeElement.textContent = hours + ':' + String(minutes).padStart(2, '0');
        }
    }
}

function calibrateZero() {
    showConfirmDialog(
        'Calibrate Empty Container',
        'Please place the empty container under the sensor, then click Confirm to calibrate.',
        function() {
            const btn = event.target;
            setButtonLoading(btn, true);

            fetch('/api/calibrate', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    showToast('Calibration set to: ' + data.zeroPoint + ' mm', 'success');
                    updateStatus();
                })
                .catch(error => showToast('Error: ' + error, 'error'))
                .finally(() => setButtonLoading(btn, false));
        }
    );
}

function calibrateDough() {
    showConfirmDialog(
        'Calibrate Fresh Starter',
        'Please place the fresh dough/starter in the container, then click Confirm to set the initial thickness.',
        function() {
            const btn = event.target;
            setButtonLoading(btn, true);

            fetch('/api/calibrate-dough', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    showToast('Dough calibrated! Initial thickness: ' + data.initialThickness + ' mm', 'success');
                    updateStatus();
                })
                .catch(error => showToast('Error: ' + error, 'error'))
                .finally(() => setButtonLoading(btn, false));
        }
    );
}

function showOffsetDialog() {
    document.getElementById('offsetDialog').style.display = 'flex';
}

function closeDialog() {
    document.getElementById('offsetDialog').style.display = 'none';
}

function saveOffset() {
    const offset = parseInt(document.getElementById('offsetInput').value) || 0;
    const btn = event.target;
    setButtonLoading(btn, true);

    fetch('/api/offset', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ offset: offset })
    })
    .then(response => response.json())
    .then(data => {
        showToast('Offset set to: ' + data.offset + ' mm', 'success');
        closeDialog();
        updateStatus();
    })
    .catch(error => showToast('Error: ' + error, 'error'))
    .finally(() => setButtonLoading(btn, false));
}

function showResetDataDialog() {
    showConfirmDialog(
        'Reset Data',
        'Delete all measurement data? This action cannot be undone.',
        function() {
            const btn = event.target;
            setButtonLoading(btn, true);

            fetch('/api/reset-data', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    showToast('Data reset complete', 'success');
                    updateStatus();
                })
                .catch(error => showToast('Error: ' + error, 'error'))
                .finally(() => setButtonLoading(btn, false));
        }
    );
}

function showResetWifiDialog() {
    showConfirmDialog(
        'Reset WiFi',
        'Reset WiFi settings? The device will reboot and you will need to reconnect.',
        function() {
            const btn = event.target;
            setButtonLoading(btn, true);

            fetch('/api/reset-wifi', { method: 'POST' })
                .then(response => response.json())
                .then(data => {
                    showToast('WiFi reset - device rebooting...', 'warning');
                })
                .catch(error => showToast('Error: ' + error, 'error'))
                .finally(() => setButtonLoading(btn, false));
        }
    );
}

function scanNetworks() {
    const scanStatus = document.getElementById('scanStatus');
    const btn = event.target;

    scanStatus.style.display = 'block';
    scanStatus.textContent = 'Scanning...';
    setButtonLoading(btn, true);

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
            showToast('Found ' + data.length + ' WiFi networks', 'success');
            setTimeout(() => { scanStatus.style.display = 'none'; }, 3000);
        })
        .catch(error => {
            scanStatus.textContent = 'Error scanning networks';
            showToast('Error scanning networks: ' + error, 'error');
            setTimeout(() => { scanStatus.style.display = 'none'; }, 3000);
        })
        .finally(() => setButtonLoading(btn, false));
}

function connectToWiFi() {
    const ssid = document.getElementById('networkSelect').value;
    const password = document.getElementById('wifiPassword').value;
    const connectStatus = document.getElementById('connectStatus');
    const btn = event.target;

    if (!ssid) {
        showToast('Please select a network', 'warning');
        return;
    }

    connectStatus.style.display = 'block';
    connectStatus.textContent = 'Connecting to ' + ssid + '...';
    setButtonLoading(btn, true);

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
            showToast('Successfully connected to ' + ssid + '!', 'success');
            setTimeout(() => {
                connectStatus.style.display = 'none';
                setTimeout(() => { location.reload(); }, 2000);
            }, 3000);
        } else {
            connectStatus.textContent = 'Connection failed: ' + (data.error || 'Unknown error');
            connectStatus.style.backgroundColor = '#ffcdd2';
            showToast('Connection failed: ' + (data.error || 'Unknown error'), 'error');
        }
    })
    .catch(error => {
        connectStatus.textContent = 'Error: ' + error;
        connectStatus.style.backgroundColor = '#ffcdd2';
        showToast('Error: ' + error, 'error');
    })
    .finally(() => setButtonLoading(btn, false));
}
)rawliteral";
}
