import chisel3._
import chisel3.util._

object Seeds {
  val seed_hash0 = 12345.U(32.W)
  val seed_hash1 = 67890.U(32.W)
  val seed_hash2 = 55555.U(32.W)
  val seed_hash3 = 99999.U(32.W)
}

object HashFunctions {
  def hash_with_seed(data: UInt, seed: UInt): UInt = {
    var tmp = data ^ seed

    tmp = (((tmp >> 16) ^ tmp) * 0x45d9f3b.U)(31, 0)
    tmp = (((tmp >> 16) ^ tmp) * 0x45d9f3b.U)(31, 0)
    tmp = (tmp >> 16) ^ tmp

    tmp(7, 0)
  }

  def hash0(data: UInt): UInt = hash_with_seed(data, Seeds.seed_hash0)
  def hash1(data: UInt): UInt = hash_with_seed(data, Seeds.seed_hash1)
  def hash2(data: UInt): UInt = hash_with_seed(data, Seeds.seed_hash2)
  def hash3(data: UInt): UInt = hash_with_seed(data, Seeds.seed_hash3)
}

class TinySketch extends Module {
  val io = IO(new Bundle {
    val value = Input(UInt(32.W))
    val update_valid = Input(Bool())
    val query_valid = Input(Bool())
    val query_result = Output(UInt(32.W))
  })

  val tables = RegInit(
    Vec(4, Vec(256, UInt(32.W))),
    VecInit(Seq.fill(4)(VecInit(Seq.fill(256)(0.U(32.W)))))
  )
  io.query_result := 0.U

  val idx0 = HashFunctions.hash0(io.value)
  val idx1 = HashFunctions.hash1(io.value)
  val idx2 = HashFunctions.hash2(io.value)
  val idx3 = HashFunctions.hash3(io.value)

  when(io.update_valid) {
    tables(0)(idx0) := tables(0)(idx0) + 1.U
    tables(1)(idx1) := tables(1)(idx1) + 1.U
    tables(2)(idx2) := tables(2)(idx2) + 1.U
    tables(3)(idx3) := tables(3)(idx3) + 1.U
  }

  when(io.query_valid) {
    val min_all = tables(0)(idx0)
      .min(tables(1)(idx1))
      .min(tables(2)(idx2))
      .min(tables(3)(idx3))
    io.query_result := min_all
  }
}

object TinySketch extends App {
  _root_.circt.stage.ChiselStage.emitSystemVerilogFile(
    gen = new TinySketch,
    args = Array("--target-dir", "../rtl"),
    firtoolOpts = Array("-disable-layers=Verification") // Полностью убирает файлы верификации
  )
}
