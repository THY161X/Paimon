use paimon_libusb::{Context, Device, Hotplug, HotplugBuilder, Registration, UsbContext};

struct HotPlugHandler {}

impl<T: UsbContext> Hotplug<T> for HotPlugHandler {
    fn device_arrived(&mut self, device: Device<T>) {
        println!("device arrived {:?}", device.device_descriptor().unwrap());
    }

    fn device_left(&mut self, device: Device<T>) {
        println!("device left {:?}", device.device_descriptor().unwrap());
    }
}

impl Drop for HotPlugHandler {
    fn drop(&mut self) {
        println!("HotPlugHandler dropped");
    }
}

pub struct USBListener {
    pub context: Context,
    pub reg_option: Option<Registration<Context>>
}

impl USBListener {
    pub fn new() -> USBListener {
        USBListener {
            context: Context::new().expect("msg"),
            reg_option: None
        }
    }

    pub fn start(&mut self) {
        if paimon_libusb::has_hotplug() {
            self.reg_option = Some(HotplugBuilder::new()
                .enumerate(true)
                .register(&self.context, Box::new(HotPlugHandler {}))
                .expect("msg"));
        } else {
            eprint!("libusb hotplug api unsupported");
        }
    }
}
