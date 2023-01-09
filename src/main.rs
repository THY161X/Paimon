use paimon_libusb::UsbContext;

mod usb_listener;

fn main() {
    let is_close = false;

    let mut ul = usb_listener::USBListener::new();
    ul.start();

    loop {
        ul.context.handle_events(None).unwrap();
        if is_close {
            break;
        }
    }
}