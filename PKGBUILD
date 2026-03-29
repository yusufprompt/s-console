# Maintainer: iuse <senin-epostan@mail.com>
pkgname=s-console
pkgver=1.0
pkgrel=1
pkgdesc="the super-term or seurce-term"
arch=('x86_64')
license=('GPL')
depends=('gtk3' 'vte3')
makedepends=('gcc' 'pkg-config')
# Yerel dosyaları kullanıyoruz (Test aşaması için)
# PKGBUILD içindeki source satırını şununla değiştir:
source=("terminal.c" "s-console.desktop" "logo.png" "s-console.conf")
sha256sums=('SKIP' 'SKIP' 'SKIP' 'SKIP')


build() {
    cd "$srcdir"
    gcc terminal.c -o s-console `pkg-config --cflags --libs gtk+-3.0 vte-2.91`
}

package() {
    cd "$srcdir"
    install -Dm755 s-console "${pkgdir}/usr/bin/s-console"
    install -Dm644 s-console.desktop "${pkgdir}/usr/share/applications/s-console.desktop"
    install -Dm644 logo.png "${pkgdir}/usr/share/icons/hicolor/512x512/apps/s-console.png"
    
    # AYAR DOSYASINI BURAYA KURUYORUZ
    install -Dm644 s-console.conf "${pkgdir}/etc/s-console/s-console.conf"
}
