<?php
session_save_path(__DIR__ . '/sessions'); // مسیر ذخیره نشست
session_start();

header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");
header("Access-Control-Allow-Methods: POST");

include_once 'db_connect.php';

$data = json_decode(file_get_contents("php://input"));

if (!empty($data->username) && !empty($data->password)) {

    $query = "SELECT * FROM users WHERE username = :username";
    $stmt = $conn->prepare($query);
    $stmt->bindParam(":username", $data->username);
    $stmt->execute();

    $user = $stmt->fetch(PDO::FETCH_ASSOC);

    if ($user && password_verify($data->password, $user['password'])) {

        $_SESSION['user_id'] = $user['id'];

        echo json_encode([
            "message" => "login_success",
            "user_id" => $user['id'],
            "username" => $user['username']
        ]);

    } else {
        echo json_encode(["message" => "نام کاربری یا رمز عبور اشتباه است."]);
    }

} else {
    echo json_encode(["message" => "اطلاعات ناقص است."]);
}
?>
