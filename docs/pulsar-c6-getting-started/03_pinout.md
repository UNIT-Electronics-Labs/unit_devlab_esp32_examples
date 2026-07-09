<script setup>
import { withBase } from 'vitepress'
</script>

# Placa de desarrollo

## Diagrama esquemático

<div style="text-align: center;">
   <a :href="withBase('/sphinx-static/nanoc6/Schematics_UNIT NANO_C6_V1_6.pdf')" target="_blank">
      <button style="background-color: #87cefa; color: white; border: none; padding: 10px 20px;">PULSAR C6</button>
   </a>
</div>
<br> </br>
<iframe :src="withBase('/sphinx-static/nanoc6/Schematics_UNIT NANO_C6_V1_6.pdf')" style="width:100%; height:500px;" frameborder="0"></iframe>
<br> </br>

## Distribución de pinout

<div style="text-align: center;">
&#10;
    <br><br>
&#10;    <!-- Imagen que se puede abrir en una nueva ventana al hacer clic -->
    <a :href="withBase('/sphinx-static/nanoc6/Nano-C6-Pinout_EN.jpg')" target="_blank">
        <img
            :src="withBase('/sphinx-static/nanoc6/Nano-C6-Pinout_EN.jpg')"
            alt="Pinout PULSAR C6"
            style="width: 90%; height: auto; border: 1px solid #ccc; border-radius: 8px;"
        >
    </a>
&#10;    <br><br>
&#10;</div>

La siguiente tabla muestra los detalles de pinout para las placas **PULSAR C6** y ESP32 C6.

<table style="width:100%;">
<colgroup>
<col style="width: 14%" />
<col style="width: 28%" />
<col style="width: 28%" />
<col style="width: 28%" />
</colgroup>
<thead>
<tr class="header">
<th>Pin Arduino Nano</th>
<th>Descripción Arduino Nano</th>
<th><strong>PULSAR C6</strong></th>
<th>ESP32 C6</th>
</tr>
</thead>
<tbody>
<tr class="odd">
<td>1</td>
<td>D13 (SCK/LED)</td>
<td>D13/SCK</td>
<td>GPIO6/A6/(SCK)</td>
</tr>
<tr class="even">
<td>2</td>
<td>3.3V</td>
<td>3.3V</td>
<td>3.3V</td>
</tr>
<tr class="odd">
<td>3</td>
<td>AREF</td>
<td><ul>
<li></li>
</ul></td>
<td>GPIO14</td>
</tr>
<tr class="even">
<td>4</td>
<td>A0 (Analógico)/D14</td>
<td>A0 /D14</td>
<td>GPIO0</td>
</tr>
<tr class="odd">
<td>5</td>
<td>A1 (Analógico)/D15</td>
<td>A1/D15</td>
<td>GPIO1</td>
</tr>
<tr class="even">
<td>6</td>
<td>A2 (Analógico)/D16</td>
<td>A2/D16</td>
<td>GPIO3</td>
</tr>
<tr class="odd">
<td>7</td>
<td>A3 (Analógico)/D17</td>
<td>A3/D17</td>
<td>GPIO4</td>
</tr>
<tr class="even">
<td>8</td>
<td>A4 (SDA)/D18</td>
<td>A4 (SDA)/D18</td>
<td>GPIO22</td>
</tr>
<tr class="odd">
<td>9</td>
<td>A5 (SCL)/D19</td>
<td>A5 (SCL)/D19</td>
<td>GPIO23</td>
</tr>
<tr class="even">
<td>10</td>
<td>A6 (Analógico)</td>
<td><ul>
<li></li>
</ul></td>
<td><ul>
<li></li>
</ul></td>
</tr>
<tr class="odd">
<td>11</td>
<td>A7 (Analógico)</td>
<td>A7</td>
<td>GPIO5</td>
</tr>
<tr class="even">
<td>12</td>
<td>5V</td>
<td>5V</td>
<td>5V</td>
</tr>
<tr class="odd">
<td>13</td>
<td>RESET</td>
<td>RST</td>
<td>RST</td>
</tr>
<tr class="even">
<td>14</td>
<td>GND</td>
<td>GND</td>
<td>GND</td>
</tr>
<tr class="odd">
<td>15</td>
<td>VIN</td>
<td>VIN</td>
<td>VIN</td>
</tr>
<tr class="even">
<td>16</td>
<td>D0 (RX)</td>
<td>D0/RX</td>
<td>GPIO17</td>
</tr>
<tr class="odd">
<td>17</td>
<td>D1 (TX)</td>
<td>D1/TX</td>
<td>GPIO16</td>
</tr>
<tr class="even">
<td>18</td>
<td>RESET</td>
<td>RST</td>
<td>RST</td>
</tr>
<tr class="odd">
<td>19</td>
<td>GND</td>
<td>GND</td>
<td>GND</td>
</tr>
<tr class="even">
<td>20</td>
<td>D2</td>
<td>D2</td>
<td>GPIO8</td>
</tr>
<tr class="odd">
<td>21</td>
<td>D3 (PWM)</td>
<td>D3/NEOP</td>
<td>GPIO9</td>
</tr>
<tr class="even">
<td>22</td>
<td>D4</td>
<td>D4</td>
<td>GPIO15</td>
</tr>
<tr class="odd">
<td>23</td>
<td>D5 (PWM)</td>
<td>D5</td>
<td>GPIO19</td>
</tr>
<tr class="even">
<td>24</td>
<td>D6 (PWM)</td>
<td>D6</td>
<td>GPIO20</td>
</tr>
<tr class="odd">
<td>25</td>
<td>D7</td>
<td>D7</td>
<td>GPIO21</td>
</tr>
<tr class="even">
<td>26</td>
<td>D8</td>
<td>D8</td>
<td>GPIO12</td>
</tr>
<tr class="odd">
<td>27</td>
<td>D9 (PWM)</td>
<td>D9</td>
<td>GPIO13</td>
</tr>
<tr class="even">
<td>28</td>
<td>D10 (PWM/SS)</td>
<td>D10/SS</td>
<td>GPIO18</td>
</tr>
<tr class="odd">
<td>29</td>
<td>D11 (PWM/MOSI)</td>
<td>D11/MOSI</td>
<td>GPIO7/(MOSI)</td>
</tr>
<tr class="even">
<td>30</td>
<td>D12 (MISO)</td>
<td>D12/MISO</td>
<td>GPIO2/A2/(MISO)</td>
</tr>
</tbody>
</table>
