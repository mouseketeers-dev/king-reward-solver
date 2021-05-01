const child_process = require('child_process');

const shouldRebuildLibraries = process.env.KING_REWARD_SOLVER_ENABLE_REBUILD;

function main() {
  if (!shouldRebuildLibraries) {
    console.log("install: Using prebuilt libraries.");
    return;
  }

  console.log("install: Building king-reward-solver...");

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
