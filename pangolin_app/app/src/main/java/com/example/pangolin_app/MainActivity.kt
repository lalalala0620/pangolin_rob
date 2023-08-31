package com.example.pangolin_app

import android.annotation.SuppressLint
import android.os.Bundle
import android.view.KeyEvent
import android.view.MotionEvent
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.pangolin_app.ui.theme.Pangolin_appTheme
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.InputStreamReader
import java.io.OutputStreamWriter
import java.net.Socket

class ClientSo(host: String, port: Int) : Socket(host, port) {
    var reader = BufferedReader(InputStreamReader(inputStream))
    private val writer = BufferedWriter(OutputStreamWriter(outputStream))
    var text = ""

    fun run(): String {
        val line = reader.readLine()
        if(line!= null)
            return line
        else
            return "fail"
    }

    fun sendData(data: String) {
        writer.write(data)
        writer.newLine() // 寫入換行符，確保伺服器可以正確讀取行
        writer.flush() // 確保資料被送出
    }
}


class MainActivity : ComponentActivity() {
    val host = "192.168.1.103"
    val port = 12345
    private lateinit var client: ClientSo
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            Pangolin_appTheme {
                // A surface container using the 'background' color from the theme
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    Greeting("穿山甲")
                }
            }
        }
        Thread(Runnable {
            client = ClientSo(host, port)
        }).start()
    }

    override fun dispatchGenericMotionEvent(event: MotionEvent?): Boolean {
        val xAxis: Float? = event?.getAxisValue(MotionEvent.AXIS_X)
        val yAxis: Float? = event?.getAxisValue(MotionEvent.AXIS_Y)
        val zAxis: Float? = event?.getAxisValue(MotionEvent.AXIS_Z)
        val rzAxis: Float? = event?.getAxisValue(MotionEvent.AXIS_RZ)
        val rtriggerAxis : Float? = event?.getAxisValue(MotionEvent.AXIS_RTRIGGER)

        val axisMap = mutableMapOf<String, Float?>()

        axisMap["xAxis"] = xAxis
        axisMap["yAxis"] = yAxis
        axisMap["zAxis"] = zAxis
        axisMap["rzAxis"] = rzAxis
        axisMap["rtriggerAxis"] = rtriggerAxis

        val jsonAxis = Json.encodeToString(axisMap)
        println(jsonAxis)
        Thread(Runnable {
                client.sendData(jsonAxis)
            }).start()
        return super.dispatchGenericMotionEvent(event)
    }
}

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    Pangolin_appTheme {
        Greeting("Android")
    }
}