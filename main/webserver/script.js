let pressButton = document.getElementById('pressButton');
let resetButton = document.getElementById('resetButton');
let counterDisplay = document.getElementById('counter');
let pressCount = 0;

// Hàm để gửi giá trị pressCount đến ESP32
function sendPressCountToESP32(count) {
    fetch('/receive', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(count)
    })
    .then(response => response.text())
    .then(data => {
        // Hiển thị giá trị từ ESP32 ở vị trí bên phải "Giá trị đo được:"
        document.getElementById("receivedValue").innerText = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });
}



// Khi người dùng nhấn giữ nút
pressButton.addEventListener('mousedown', () => {
    pressButton.textContent = 'Release';
    pressCount++;
    counterDisplay.textContent = `Press Counter: ${pressCount}`;

    // Gửi giá trị pressCount đến ESP32 khi mousedown
    sendPressCountToESP32(pressCount);
});

// Khi người dùng thả nút
pressButton.addEventListener('mouseup', () => {
    pressButton.textContent = 'Press';
});

// Nút để xóa số lần nhấn
resetButton.addEventListener('click', () => {
    pressCount = 0;
    counterDisplay.textContent = `Press Counter: ${pressCount}`;
    sendPressCountToESP32(pressCount);
});
