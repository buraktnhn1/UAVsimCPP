UAV Tactical Simulation Engine
Bu proje, C++ ve OpenCV kullanılarak geliştirilmiş bir İHA yer kontrol istasyonu simülasyonudur.

🛠 Gereksinimler
Derleyici: Visual Studio (C++ Desktop Development yüklü olmalıdır).

Kütüphane: OpenCV (4.x sürümü önerilir).

⚙️ Kurulum ve Ayarlar
Projeyi çalıştırmak için OpenCV kütüphanesini Visual Studio'ya şu şekilde bağlamalısın:

Include Path: Proje Özellikleri -> C/C++ -> General -> Additional Include Directories kısmına OpenCV include klasörünün yolunu ekle.

Library Path: Proje Özellikleri -> Linker -> General -> Additional Library Directories kısmına OpenCV lib klasörünün yolunu ekle.

Dependencies: Proje Özellikleri -> Linker -> Input -> Additional Dependencies kısmına kullandığın OpenCV sürümünün .lib dosyasını (örneğin: opencv_world412.lib) ekle.

DLL: opencv_world412.dll dosyasını projenin .exe dosyasının olduğu klasöre kopyalamayı unutma.

🚀 Çalıştırma
Projeyi Visual Studio'da UAVsim.sln dosyası ile aç.

Konfigürasyonu x64 moduna getir.

"Local Windows Debugger" butonuna basarak simülasyonu başlat.                                      
