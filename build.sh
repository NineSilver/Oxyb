rm -rf thirdparty/limine
git clone --depth 1 --branch=v2.0-binary https://github.com/limine-bootloader/limine thirdparty/limine
rm -rf image
rm -f *.iso

cd kernel
	sh build.sh
cd ..

mkdir -p image/boot
cp thirdparty/limine/limine.sys              image/boot
cp thirdparty/limine/limine-cd.bin           image/boot
cp thirdparty/limine/limine-eltorito-efi.bin image/boot
cp kernel/oxyb.elf                           image/boot
echo "TIMEOUT=5"                          > image/boot/limine.cfg
echo "GRAPHICS=yes"                      >> image/boot/limine.cfg
echo ":Oxyb"                             >> image/boot/limine.cfg
echo "PROTOCOL=stivale2"                 >> image/boot/limine.cfg
echo "KERNEL_PATH=boot:///boot/oxyb.elf" >> image/boot/limine.cfg

xorriso \
	-as mkisofs \
	-b boot/limine-cd.bin \
	-no-emul-boot \
	-boot-load-size `nproc --all` \
	-boot-info-table \
	--efi-boot boot/limine-eltorito-efi.bin \
	-efi-boot-part \
	--efi-boot-image \
	--protective-msdos-label \
	image \
	-o oxyb.iso

thirdparty/limine/limine-install-linux-x86_64 oxyb.iso

qemu-system-x86_64 -m 256M -machine q35 -no-reboot -no-shutdown -serial stdio -hda oxyb.iso
