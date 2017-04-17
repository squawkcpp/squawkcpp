squawkApp.factory('Reddit', function($http) {
  var Reddit = function() {
    this.items = [];
    this.busy = false;
    this.after = 0;
    this.filterResults = [];
  };

  Reddit.prototype.nextPage = function() {
    if (this.busy) return;
    this.busy = true;

    console.log( "after:" +  this.after );

    $http.get( "/query?index=" + this.after + "&count=128&&classes="+this.filterResults ).then(function (success) {
        console.log( "new model:" + success.data.length );
        var items = success.data;
        for (var i = 0; i < items.length; i++) {
//            console.log( " ....:" + items );
                  this.items.push(items[i]);
//        this.items = items;
        }
        this.busy = false;
        this.after = this.items.length;
    }/*,function (error){
        console.log("error get albums: " + error );
        this.busy = false;
      } */.bind(this));
    };

/*    var url = "https://api.reddit.com/hot?after=" + this.after + "&jsonp=JSON_CALLBACK";
    $http.get( url ).then(function(data) {
      var items = data;
        alert( items );
      for (var i = 0; i < items.length; i++) {
        this.items.push(items[i].data);
      }
      this.after = "t3_" + this.items[this.items.length - 1].id;
      this.busy = false;
    }.bind(this));
  };
*/
  return Reddit;
});
squawkApp.controller('ContentDirectoryController', ['$scope', '$http', 'Reddit', function($scope, $http, Reddit) {
    $scope.filterModel = {
        album: true,
        serie: false,
        movie: false,
        ebook: false,
        image: false
      };
    $scope.filterResults = [];
    $scope.reddit = new Reddit();

      $scope.$watchCollection('filterModel', function () {
          console.log( "$scope.filterResults changed" );
          $scope.filterResults = [];
          $scope.reddit.filterResults = [];
          $scope.reddit.items = [];

        angular.forEach($scope.filterModel, function (value, key) {
          if (value) {
              $scope.filterResults.push(key);
              $scope.reddit.filterResults.push(key);
          }
        });
          console.log( $scope.filterResults );
          $scope.reddit.after = 0;
          $scope.reddit.nextPage();
          //$scope.refresh();

      });

    /* $scope.pageChanged = function(newPage) {
        alert(newPage);
    }; */

/*    $scope.refresh = function() {
        console.log( "refresh model:" + $scope.filterResults );
        $http.get("/query?index=0&count=128&classes="+$scope.filterResults)
        .then(function (success) {
            console.log( "new model:" + success.data.length );
            $scope.items = success.data;
        },function (error){
            console.log("error get albums: " + error );
    });
  }; */

}]);

/*
squawkApp.controller('FilterController', ['$routeParams', '$scope', '$http',  function($routeParams, $scope, $http) {
    $scope.cdsResults = this;
    $scope.filterModel = {
        album: true,
        serie: false,
        movie: false,
        ebook: false,
        image: false
      };
    $scope.filterResults = [];

      $scope.$watchCollection('filterModel', function () {
          console.log( "collection changed");
        $scope.filterResults = [];
        angular.forEach($scope.filterModel, function (value, key) {
          if (value) {
            $scope.filterResults.push(key);
          }
        });
      });
}]);
*/
squawkApp.controller('AlbumController', ['$routeParams', '$scope', '$http',  function($routeParams, $scope, $http) {
    $scope.refresh = function() {
        $http.get("/album/" + $routeParams.albumId )
        .then(function (success) {
            $scope.album = success.data;
        },function (error){
            console.log("error get albums: " + error );
    });
  };
}]);
squawkApp.controller('BookController', ['$routeParams', '$scope', '$http',  function($routeParams, $scope, $http) {
    $scope.refresh = function() {
        $http.get("/book/" + $routeParams.ebookId )
        .then(function (success) {
            $scope.book = success.data;
        },function (error){
            console.log("error get book: " + error );
    });
  };
}]);
squawkApp.controller('SeriesController', ['$routeParams', '$scope', '$http',  function($routeParams, $scope, $http) {
    $scope.refresh = function() {
        $http.get("/series/" + $routeParams.seriesId )
        .then(function (success) {
            $scope.series = success.data;
        },function (error){
            console.log("error get series: " + error );
    });
  };
}]);
squawkApp.controller('MovieItemCtrl', ['$routeParams', '$scope', '$http', '$sce', function ($routeParams, $scope, $http, $sce) {
    var controller = this;
    $scope.refresh = function() {
        $http.get("/movie/" + $routeParams.movieId )
        .then(function (success) {
            $scope.movie = success.data;
            controller.config.sources = [
                     {src: $sce.trustAsResourceUrl(success.data.uri), type: "video/mp4"},
                 ];
        },function (error){
            console.log("error get video: " + error );
    });
    };
    /* this.config = {
        sources: [
            {src: $sce.trustAsResourceUrl("http://static.videogular.com/assets/videos/videogular.mp4"), type: "video/mp4"},
            {src: $sce.trustAsResourceUrl("http://static.videogular.com/assets/videos/videogular.webm"), type: "video/webm"},
            {src: $sce.trustAsResourceUrl("http://static.videogular.com/assets/videos/videogular.ogg"), type: "video/ogg"}
        ],
        tracks: [
            {
                src: "http://www.videogular.com/assets/subs/pale-blue-dot.vtt",
                kind: "subtitles",
                srclang: "en",
                label: "English",
                default: ""
            }
        ],
        theme: "/bower_components/videogular-themes-default/videogular.css",
        plugins: {
            poster: "http://www.videogular.com/assets/images/videogular.png"
        }
    }; */

}]);
