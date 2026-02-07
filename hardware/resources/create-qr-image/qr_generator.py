import requests
import qrcode

original_link = "github.com/MrR0b0t19/APT-V114"

qr = qrcode.QRCode(
    version=1,  # tamaño pequeño
    error_correction=qrcode.constants.ERROR_CORRECT_L,
    box_size=8,
    border=1,
)
qr.add_data(original_link)
qr.make(fit=True)

img = qr.make_image(fill_color="black", back_color="white")
img.save("qr_link_corto.png")

print("QR generado en qr_link_corto.png")