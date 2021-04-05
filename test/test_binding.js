const Solver = require("../lib/binding.js");
const assert = require("assert");
const path = require("path");
const fs = require("fs").promises;

assert(Solver, "The expected function is undefined");

function testBasic() {
  const result = Solver.openCvVersion();
  assert.strictEqual(result, "4.5.1", "Unexpected value returned");
}

async function testInitialize() {
  const result = await Solver.initialize();
  assert.strictEqual(result, "2346789BCDFGHJMNPQRTVWXYZbcdfghjmnpqrstvwxyz", "Unexpected value returned");
}

async function testSolve() {
  const result = await Solver.solve(path.resolve(__dirname, "puzzle_test.png"));
  assert.strictEqual(result, "gqFj6", "Unexpected value returned");
}

async function testSolveBuffer() {
  const buffer = await fs.readFile(path.resolve(__dirname, "puzzle_test_2.png"));
  const result = await Solver.solve(buffer);
  assert.strictEqual(result, "WNnTr", "Unexpected value returned");
}

(async () => {
  assert.doesNotThrow(testBasic, undefined, "testBasic threw an exception");
  await assert.doesNotReject(testInitialize, undefined, "testInitialize rejected");
  await assert.doesNotReject(testSolve, undefined, "testSolve rejected");
  await assert.doesNotReject(testSolveBuffer, undefined, "testSolveBuffer rejected");

  console.log("Tests passed- everything looks OK!");

})();
