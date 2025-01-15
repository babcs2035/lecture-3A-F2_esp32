#include <IOKit/IOKitLib.h>
#include <IOKit/pwr_mgt/IOPowerSources.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>

void print_battery_info() {
    io_iterator_t iterator;
    io_object_t power_source;

    // バッテリーの情報を取得
    CFMutableDictionaryRef matchingDict = IOServiceMatching("AppleSmartBattery");
    kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, &iterator);

    if (result != KERN_SUCCESS) {
        printf("バッテリー情報の取得に失敗しました。\n");
        return;
    }

    // バッテリーの詳細情報を取得
    while ((power_source = IOIteratorNext(iterator))) {
        CFTypeRef current_capacity = IORegistryEntryCreateCFProperty(power_source, CFSTR("CurrentCapacity"), kCFAllocatorDefault, 0);
        CFTypeRef max_capacity = IORegistryEntryCreateCFProperty(power_source, CFSTR("MaxCapacity"), kCFAllocatorDefault, 0);
        CFTypeRef is_charging = IORegistryEntryCreateCFProperty(power_source, CFSTR("IsCharging"), kCFAllocatorDefault, 0);
        CFTypeRef charging_current = IORegistryEntryCreateCFProperty(power_source, CFSTR("ChargingCurrent"), kCFAllocatorDefault, 0);
        CFTypeRef charging_power = IORegistryEntryCreateCFProperty(power_source, CFSTR("ChargingPower"), kCFAllocatorDefault, 0);

        if (current_capacity) {
            printf("Current Capacity: %ld mAh\n", CFNumberGetValue(current_capacity, kCFNumberSInt32Type, NULL));
            CFRelease(current_capacity);
        }
        if (max_capacity) {
            printf("Max Capacity: %ld mAh\n", CFNumberGetValue(max_capacity, kCFNumberSInt32Type, NULL));
            CFRelease(max_capacity);
        }
        if (is_charging) {
            printf("Is Charging: %s\n", CFBooleanGetValue(is_charging) ? "Yes" : "No");
            CFRelease(is_charging);
        }
        if (charging_current) {
            printf("Charging Current: %ld mA\n", CFNumberGetValue(charging_current, kCFNumberSInt32Type, NULL));
            CFRelease(charging_current);
        }
        if (charging_power) {
            printf("Charging Power: %ld mW\n", CFNumberGetValue(charging_power, kCFNumberSInt32Type, NULL));
            CFRelease(charging_power);
        }

        IOObjectRelease(power_source);
    }

    IOObjectRelease(iterator);
}

int main() {
    print_battery_info();
    return 0;
}
