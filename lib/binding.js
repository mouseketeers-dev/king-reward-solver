const os = require('os');
const osName = os.platform() === "win32" ? "win" : os.platform();

const addon = require(`./native/${osName}/king-reward-solver-native`);
const path = require("path");

module.exports = {
  openCvVersion: addon.openCvVersion,

  initialize: function (modelPath = "model.xml") {
    return addon.initialize(path.resolve(__dirname, modelPath));
  },

  solve: function (captcha) {
    return addon.solve(captcha);
  }
};

