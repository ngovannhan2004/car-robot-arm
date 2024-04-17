# Dự án Xe Điều Khiển Bằng ESP32

## Giới thiệu
Dự án này minh họa cách điều khiển một chiếc xe sử dụng vi điều khiển ESP32 kết nối với driver động cơ H-Bridge L298N để điều khiển hai động cơ DC. Thiết lập này cho phép thực hiện các thao tác cơ bản như di chuyển tới, lui và quay.

## Các thành phần cần thiết
- Mô-đun ESP32 Dev
- Driver động cơ L298N H-Bridge
- 2x Động cơ DC
- Nguồn điện phù hợp (thường là 12V cho L298N và 3.3V hoặc 5V cho ESP32)
- Dây nhảy
- Breadboard (tùy chọn cho việc lắp ráp thử nghiệm)

## Sơ đồ kết nối
**Vui lòng chèn đường dẫn tới hình ảnh sơ đồ kết nối của bạn ở đây, ví dụ:**

### Hướng dẫn kết nối

1. **Kết nối từ L298N tới ESP32:**
   - `pinLeftEN` (ESP32) → `ENB` (L298N)
   - `pinRightEN` (ESP32) → `ENA` (L298N)
   - `pinLeftIN1` (ESP32) → `IN3` (L298N)
   - `pinLeftIN2` (ESP32) → `IN4` (L298N)
   - `pinRightIN1` (ESP32) → `IN1` (L298N)
   - `pinRightIN2` (ESP32) → `IN2` (L298N)

2. **Kết nối từ L298N tới động cơ DC:**
   - Kết nối `OUT1` và `OUT2` với động cơ bên phải.
   - Kết nối `OUT3` và `OUT4` với động cơ bên trái.

3. **Nguồn điện:**
   - Kết nối nguồn điện vào `12V` và `GND` trên L298N.
   - Đảm bảo rằng ESP32 được cung cấp điện an toàn qua USB hoặc nguồn điện ngoài phù hợp.

4. **Mặt đất chung:**
   - Kết nối `GND` của ESP32 với `GND` của L298N để hoàn thành mạch mặt đất chung.

## Cài đặt và thiết lập
1. Cài đặt Arduino IDE và cấu hình cho ESP32 bằng cách cài đặt các định nghĩa bo mạch ESP32.
2. Tải lên sketch `ControlCar` được cung cấp trong thư mục `src` của kho lưu trữ này.

## Cách sử dụng
- **Di chuyển tới:** `car.moveForward();`
- **Di chuyển lui:** `car.moveBackward();`
- **Quay trái:** `car.turnLeft();`
- **Quay phải:** `car.turnRight();`
- **Dừng lại:** `car.stop();`

## Tùy chỉnh
Bạn có thể điều chỉnh tốc độ động cơ bằng cách sửa đổi các phương thức `setSpeedMove` và `setSpeedTurn`. Các giá trị chấp nhận từ 0 (dừng) đến 255 (tốc độ tối đa).

## Khắc phục sự cố
- Kiểm tra tất cả các kết nối để đảm bảo chúng chắc chắn và đúng.
- Xác minh rằng nguồn điện có khả năng cung cấp đủ điện áp và dòng điện mà không gây hại cho các thành phần.
- Đảm bảo rằng ESP32 và L298N không chịu điện áp cao hơn so với thông số kỹ thuật của chúng.

## Tác giả
- Nam077

## Bản quyền
Dự án này được cấp phép theo Giấy phép MIT - xem tệp LICENSE.md để biết chi tiết.
