<?php
session_save_path(__DIR__ . '/sessions');
session_start();

if (!isset($_SESSION['user_id'])) {
    http_response_code(403);
    echo json_encode(["message" => "دسترسی غیرمجاز! لطفا ابتدا وارد شوید."]);
    exit();
}
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");
header("Access-Control-Allow-Methods: POST");

include_once 'db_connect.php';

$data = json_decode(file_get_contents("php://input"));

if (!empty($data->id) && !empty($data->title) && !empty($data->content)) {
    try {
        // آپدیت کردن پست بر اساس ID
        $query = "UPDATE posts SET title = :title, content = :content, summary = :summary WHERE id = :id";
        $stmt = $conn->prepare($query);

        $stmt->bindParam(":title", $data->title);
        $stmt->bindParam(":content", $data->content);
        $stmt->bindParam(":summary", $data->summary);
        $stmt->bindParam(":id", $data->id);

        if ($stmt->execute()) {
            echo json_encode(["message" => "post_updated"]);
        } else {
            echo json_encode(["message" => "خطا در آپدیت دیتابیس."]);
        }
    } catch (Exception $e) {
        echo json_encode(["message" => "خطا: " . $e->getMessage()]);
    }
} else {
    echo json_encode(["message" => "اطلاعات ناقص است."]);
}
?>
