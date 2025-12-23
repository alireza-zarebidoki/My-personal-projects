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

if (!empty($data->id)) {
    $query = "DELETE FROM posts WHERE id = :id";
    $stmt = $conn->prepare($query);
    $stmt->bindParam(":id", $data->id);

    if ($stmt->execute()) {
        echo json_encode(["message" => "post_deleted"]);
    } else {
        echo json_encode(["message" => "خطا در حذف پست."]);
    }
}
?>
