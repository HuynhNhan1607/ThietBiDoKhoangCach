let dataArray = []; // Biến lưu trữ dữ liệu nhận từ server
let dateArray = []; // Biến lưu trữ thời gian nhận từ server

// Hàm gửi yêu cầu POST tới /receive để nhận mảng dữ liệu từ server
function fetchDataFromServer(action) {
    fetch('/receive', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain' // Thay đổi Content-Type nếu chỉ gửi chuỗi
        },
        body: action // Gửi chuỗi trực tiếp thay vì JSON
    })
    .then(response => response.json())
    .then(data => {
        // Kiểm tra nếu data.data và data.date là mảng và có ít nhất 5 phần tử
        if (Array.isArray(data.data) && data.data.length >= 5 &&
            Array.isArray(data.date) && data.date.length >= 5) {
            dataArray = data.data; // Lưu dữ liệu đo đạc
            dateArray = data.date; // Lưu thời gian tương ứng
            if (action === "Measuring") {
                displaySingleElement(0); // Chỉ hiển thị phần tử đầu tiên khi nhấn "Measuring"
            } else if (action === "History") {
                displayHistoryElements(); // Hiển thị box `History` khi nhấn "History"
            }
        } else {
            dataArray = []; // Đặt lại mảng rỗng nếu dữ liệu không đủ phần tử
            dateArray = []; // Đặt lại mảng rỗng nếu thời gian không đủ phần tử
            if (action === "History") displayHistoryElements(); // Chỉ hiện thông báo khi nhấn "History"
        }
    })
    .catch((error) => {
        console.error('Error:', error);
        dataArray = []; // Đặt lại mảng rỗng nếu có lỗi
        dateArray = []; // Đặt lại mảng rỗng nếu có lỗi
        if (action === "History") displayHistoryElements(); // Chỉ hiện thông báo khi nhấn "History"
    });
}

// Hiển thị một phần tử duy nhất (phần tử đầu tiên trong `Distance`), thêm đơn vị "mm"
function displaySingleElement(index) {
    const element = dataArray[index];
    const date = dateArray[index];
    if (element !== undefined && date !== undefined) {
        document.getElementById('counter').innerText = `${element} mm`; // Thêm đơn vị "mm"
        document.getElementById('receivedValue').innerText = date; // Hiển thị thời gian tương ứng
    }
}

// Hiển thị các phần tử còn lại trong `historyBox` khi nhấn "History", thêm đơn vị "mm" và thời gian
function displayHistoryElements() {
    const historyBox = document.getElementById('historyBox');
    const historyList = document.getElementById('historyList');
    historyList.innerHTML = ""; // Xóa danh sách cũ

    // Lọc và hiển thị các phần tử không phải là 0
    const filteredData = dataArray.slice(1).filter((element, index) => element !== 0);
    const filteredDates = dateArray.slice(1).filter((date, index) => dataArray[index + 1] !== 0);

    if (filteredData.length > 0) {
        // Thêm các phần tử vào danh sách với đơn vị "mm" và thời gian
        filteredData.forEach((element, index) => {
            const listItem = document.createElement('li');
            listItem.textContent = `${element} mm - ${filteredDates[index]}`; // Thêm đơn vị "mm" và thời gian
            historyList.appendChild(listItem);
        });
    } else {
        // Hiển thị thông báo nếu không có dữ liệu lịch sử
        const listItem = document.createElement('li');
        listItem.textContent = "Không có dữ liệu lịch sử";
        historyList.appendChild(listItem);
    }

    // Chỉ hiển thị `historyBox` khi nhấn nút "History"
    historyBox.style.display = 'block';
}

// Ẩn `historyBox` khi nhấn "Close"
document.getElementById('closeHistoryButton').addEventListener('click', () => {
    document.getElementById('historyBox').style.display = 'none';
});

// Xóa nội dung khi nhấn nút "Clear"
document.getElementById('clearButton').addEventListener('click', () => {
    document.getElementById('receivedValue').innerText = "Đang tải..."; // Đặt lại DATE
    document.getElementById('counter').innerText = "Đang tải..."; // Đặt lại Distance
    document.getElementById('historyList').innerHTML = ""; // Xóa nội dung box `History`
    document.getElementById('historyBox').style.display = 'none'; // Ẩn box `History`
    dataArray = []; // Đặt lại mảng dữ liệu
    dateArray = []; // Đặt lại mảng thời gian

    // Gửi yêu cầu xóa dữ liệu tới server
    fetch('/receive', {
        method: 'POST',
        headers: {
            'Content-Type': 'text/plain' // Thay đổi Content-Type
        },
        body: "Clear" // Gửi chuỗi trực tiếp thay vì JSON
    })
    .then(response => response.json())
    .then(data => {
        console.log("Clear response:", data); // Xác nhận phản hồi từ server (nếu có)
    })
    .catch(error => {
        console.error("Error sending clear request:", error);
    });
});

// Gán sự kiện cho nút "Measuring"
document.getElementById('measuringButton').addEventListener('click', () => {
    fetchDataFromServer("Measuring"); // Gửi yêu cầu nhận mảng và chỉ hiển thị phần tử đầu tiên
});

// Gán sự kiện cho nút "History"
document.getElementById('historyButton').addEventListener('click', () => {
    fetchDataFromServer("History"); // Gửi yêu cầu nhận mảng và hiển thị lịch sử
});
