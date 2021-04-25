const log = require('npmlog');
const child_process = require('child_process');

const shouldRebuildLibraries = process.env.ENABLE_REBUILD;

function main() {
  if (!shouldRebuildLibraries) {
    log.info("install", "Using prebuilt libraries.");
    return;
  }

  log.info("install", "Building king-reward-solver...");

  const child = child_process.exec(
    "node-gyp rebuild",
    { maxBuffer: Infinity }, function (err, stdout, stderr) {
      if (err) throw(err);
    }
  );

  child.stdout.pipe(process.stdout);
  child.stderr.pipe(process.stderr);
}

main();
