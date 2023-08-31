import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.Socket


class ClientSo(host: String, port: Int) : Socket(host, port) {
    var reader = BufferedReader(InputStreamReader(inputStream))
    var text = ""

    fun run(): String {
        val line = reader.readLine()
        if(line!= null)
            return line
        else
            return "fail"
    }
}
