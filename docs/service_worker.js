// キャッシュファイルの指定
var CACHE_NAME = 'cbm-pwa-caches';
const urlsToCache = [
    './',
    './index.html',
    './style.css',
    './display.js',
    './bluejelly.js',
    './BLE.js',
];

// インストール処理
self.addEventListener('install', function (event) {
    event.waitUntil(
        caches
            .open(CACHE_NAME)
            .then(function (cache) {
                return cache.addAll(urlsToCache);
            })
    );
});

// リソースフェッチ時のキャッシュロード処理
self.addEventListener('fetch', function (event) {
    event.respondWith(
        caches
            .match(event.request)
            .then(function (response) {
                return response ? response : fetch(event.request);
            })
    );
});