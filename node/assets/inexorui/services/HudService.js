define(['./module'], function(services) {
  return services.factory('HudService', [ 'InexorUserInterfaceService', function(InexorUserInterfaceService) {

    // The list of available HUDs.
    var huds = {};

    var updateHudState = function() {
      var visible = false;
      var acceptKeyInput = false;
      for (var hud in huds) {
        if (hud.visible) {
          visible = true;
        }
        if (hud.acceptKeyInput) {
        	acceptKeyInput = true;
        }
      }
      InexorUserInterfaceService.setState(visible, acceptKeyInput);
    };

    var showHud = function(name) {
      angular.element('#hud-' + name).css('width', huds[name].w);
      angular.element('#hud-' + name).css('height', huds[name].h);
      angular.element('#hud-' + name).css('display', 'inline');
      huds[name].visible = true;
      updateHudState();
      console.log("Showing HUD: " + name);
    };

    var hideHud = function(name) {
      angular.element('#hud-' + name).css('width', '0px');
      angular.element('#hud-' + name).css('height', '0px');
      angular.element('#hud-' + name).css('display', 'none');
      huds[name].visible = false;
      console.log("Hiding HUD: " + name);
    };

    var toggleHud = function(name) {
      if (huds[name].visible) {
        hideHud(name);
      } else {
        showHud(name);
      }
    };

    var registerHud = function(name, visible, acceptKeyInput, x, y, w, h, defaultHotkey) {
      huds[name] = {
        name: name,
        visible: visible,
        acceptKeyInput: acceptKeyInput,
        x: x,
        y: y,
        w: w,
        h: h,
        defaultHotkey: defaultHotkey
      };
      if (visible) {
        showHud(name);
      } else {
        hideHud(name);
      }
      console.log("Registered HUD: " + name);
      console.log(huds);
    };

    return {
      registerHud: registerHud,
      showHud: showHud,
      hideHud: hideHud,
      toggleHud: toggleHud
    };

  }]);
});
