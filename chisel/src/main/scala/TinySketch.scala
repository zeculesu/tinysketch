import chisel3._
import chisel3.util._
import scala.io.Source

object Seeds {
  private def readSeedsFromJson(filePath: String): Seq[UInt] = {
    try {
      val source = Source.fromFile(filePath)
      val content = source.mkString
      source.close()

      val start = content.indexOf('[')
      val end = content.indexOf(']', start)
      if (start != -1 && end != -1) {
        val arr = content.substring(start + 1, end)
        arr
          .split(',')
          .map(_.trim)
          .filter(_.nonEmpty)
          .map { token =>
            token.toLong.U(32.W)
          }
          .toSeq
      } else {
        Seq.empty[UInt]
      }
    } catch {
      case e: Exception =>
        println(
          s"Warning: Could not read seeds from $filePath: ${e.getMessage}"
        )
        Seq.empty[UInt]
    }
  }

  val seedsFromJson = readSeedsFromJson("../config/seeds.json")
  val allSeeds = seedsFromJson
}

object HashFunctions {
  def hash(value: UInt, seed: UInt, columns: Int, counterWidth: Int): UInt = {
    var tmp = value ^ seed

    tmp = (((tmp >> 16) ^ tmp) * 0x45d9f3b.U)(counterWidth - 1, 0)
    tmp = (((tmp >> 16) ^ tmp) * 0x45d9f3b.U)(counterWidth - 1, 0)
    tmp = (tmp >> 16) ^ tmp
    tmp = tmp(counterWidth - 1, 0)

    tmp % columns.U
  }
}

class TinySketch(val Rows: Int, val Columns: Int, val CounterWidth: Int)
    extends Module {
  val io = IO(new Bundle {
    val value = Input(UInt(CounterWidth.W))
    val update_valid = Input(Bool())
    val query_valid = Input(Bool())
    val query_result = Output(UInt(CounterWidth.W))
  })

  val tables = RegInit(
    VecInit(Seq.fill(Rows)(VecInit(Seq.fill(Columns)(0.U(CounterWidth.W)))))
  )

  io.query_result := 0.U

  val seeds: Seq[UInt] = Seeds.allSeeds.take(Rows)

  when(io.update_valid) {
    for (i <- 0 until Rows) {
      val idx = HashFunctions.hash(io.value, seeds(i), Columns, CounterWidth)
      tables(i)(idx) := tables(i)(idx) + 1.U
    }
  }

  when(io.query_valid) {
    var minVal =
      tables(0)(HashFunctions.hash(io.value, seeds(0), Columns, CounterWidth))
    for (i <- 1 until Rows) {
      val idx = HashFunctions.hash(io.value, seeds(i), Columns, CounterWidth)
      minVal = Mux(minVal < tables(i)(idx), minVal, tables(i)(idx))
    }
    io.query_result := minVal
  }
}

object TinySketch extends App {
  val rows = if (args.length > 0) args(0).toInt else 4
  val columns = if (args.length > 1) args(1).toInt else 256
  val counterWidth = if (args.length > 2) args(2).toInt else 32

  println(s"Generating TinySketch with:")
  println(s"  Rows: $rows")
  println(s"  Columns: $columns")
  println(s"  CounterWidth: $counterWidth")

  _root_.circt.stage.ChiselStage.emitSystemVerilogFile(
    gen = new TinySketch(rows, columns, counterWidth),
    args = Array("--target-dir", "../rtl"),
    firtoolOpts = Array("-disable-layers=Verification")
  )
}
