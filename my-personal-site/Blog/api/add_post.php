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

if (!empty($data->title) && !empty($data->content)) {
    try {
        // ساخت Slug (آدرس یکتا) برای URL
        $slug = str_replace(' ', '-', $data->title) . '-' . time();

        $query = "INSERT INTO posts (title, slug, content, summary, created_at) VALUES (:title, :slug, :content, :summary, NOW())";
        $stmt = $conn->prepare($query);

        $stmt->bindParam(":title", $data->title);
        $stmt->bindParam(":slug", $slug);
        $stmt->bindParam(":content", $data->content);
        $stmt->bindParam(":summary", $data->summary);

        if ($stmt->execute()) {
            echo json_encode(["message" => "post_created"]);
        } else {
            echo json_encode(["message" => "خطا در ثبت دیتابیس."]);
        }
    } catch (Exception $e) {
        echo json_encode(["message" => "خطا: " . $e->getMessage()]);
    }
} else {
    echo json_encode(["message" => "عنوان و متن نمی‌تواند خالی باشد."]);
}
?>
