var squawkApp = angular.module('squawkApp', [
'ngRoute',
'ngSanitize',
'ui.bootstrap',
"com.2fdevs.videogular",
"com.2fdevs.videogular.plugins.controls",
"com.2fdevs.videogular.plugins.overlayplay",
"com.2fdevs.videogular.plugins.poster",
"infinite-scroll"
]);
squawkApp.config(['$routeProvider',
function($routeProvider) {
$routeProvider.
when('/query', {
templateUrl: 'templates/query.html',
controller: 'ContentDirectoryController'
}).
when('/object.container.album.musicAlbum/:albumId.html', {
templateUrl: 'templates/album-item.html',
controller: 'AlbumController'
}).
when('/object.item.ebook/:ebookId.html', {
templateUrl: 'templates/book-item.html',
controller: 'BookController'
}).
when('/object.item.movie/:movieId.html', {
templateUrl: 'templates/movie-item.html',
controller: 'MovieItemCtrl'
}).
when('/object.container.series/:seriesId.html', {
templateUrl: 'templates/series.html',
controller: 'SeriesController'
}).
when('/admin', {
templateUrl: 'templates/admin.html',
controller: 'AdminCtrl'
}).
otherwise({
redirectTo: '/query'
});
}]);
