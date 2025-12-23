<?php
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");

include_once 'db_connect.php';

try {
    // 1. اگر ID درخواست شده باشد (برای نمایش تکی)
    if (isset($_GET['id'])) {
        $query = "SELECT * FROM posts WHERE id = :id";
        $stmt = $conn->prepare($query);
        $stmt->bindParam(":id", $_GET['id']);
        $stmt->execute();
        $post = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($post) echo json_encode($post);
        else echo json_encode(["message" => "پست پیدا نشد."]);

    }
    // 2. اگر TAG درخواست شده باشد (برای جستجو)
    elseif (isset($_GET['tag'])) {
        $tag = $_GET['tag'];
        // جستجو میکنیم که آیا این تگ در متن یا عنوان یا خلاصه وجود دارد؟
        // از % استفاده میکنیم تا هر جای متن بود پیدا کند
        $query = "SELECT id, title, slug, summary, image_url, created_at FROM posts
                  WHERE content LIKE :tag OR summary LIKE :tag OR title LIKE :tag
                  ORDER BY created_at DESC";

        $stmt = $conn->prepare($query);
        $searchTag = "%#$tag%"; // مثلاً %#فلسفه%
        $stmt->bindParam(":tag", $searchTag);
        $stmt->execute();

        $posts = $stmt->fetchAll(PDO::FETCH_ASSOC);
        echo json_encode($posts);
    }
    // 3. اگر هیچکدام نبود (نمایش همه در صفحه اصلی)
    else {
        $query = "SELECT id, title, slug, summary, image_url, created_at FROM posts ORDER BY created_at DESC";
        $stmt = $conn->prepare($query);
        $stmt->execute();
        $posts = $stmt->fetchAll(PDO::FETCH_ASSOC);
        echo json_encode($posts);
    }

} catch (Exception $e) {
    echo json_encode(["message" => "خطا: " . $e->getMessage()]);
}
?>
