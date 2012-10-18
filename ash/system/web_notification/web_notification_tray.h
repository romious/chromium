// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_SYSTEM_WEB_NOTIFICATION_WEB_NOTIFICATION_TRAY_H_
#define ASH_SYSTEM_WEB_NOTIFICATION_WEB_NOTIFICATION_TRAY_H_

#include "ash/ash_export.h"
#include "ash/system/tray/tray_background_view.h"
#include "ash/system/tray/tray_views.h"
#include "ash/system/user/login_status.h"
#include "ash/system/web_notification/web_notification_list.h"
#include "base/gtest_prod_util.h"

namespace aura {
class LocatedEvent;
}

namespace gfx {
class ImageSkia;
}

namespace views {
class ImageButton;
}

namespace message_center {
class MessageCenterBubble;
class PopupBubble;
}

namespace ash {

namespace internal {
class StatusAreaWidget;
class WebNotificationBubbleWrapper;
}

// Status area tray for showing browser and app notifications. The client
// (e.g. Chrome) calls [Add|Remove|Update]Notification to create and update
// notifications in the tray. It can also implement Delegate to receive
// callbacks when a notification is removed (closed), clicked on, or a menu
// item is triggered.
//
// Note: These are not related to system notifications (i.e NotificationView
// generated by SystemTrayItem). Visibility of one notification type or other
// is controlled by StatusAreaWidget.

class ASH_EXPORT WebNotificationTray
    : public internal::TrayBackgroundView,
      public message_center::TrayBubbleView::Delegate,
      public message_center::WebNotificationList::Delegate,
      public views::ButtonListener {
 public:
  class ASH_EXPORT Delegate {
   public:
    virtual ~Delegate() {}

    // Called when the notification associated with |notification_id| is
    // removed (i.e. closed by the user).
    virtual void NotificationRemoved(const std::string& notifcation_id) = 0;

    // Request to disable the extension associated with |notification_id|.
    virtual void DisableExtension(const std::string& notifcation_id) = 0;

    // Request to disable notifications from the source of |notification_id|.
    virtual void DisableNotificationsFromSource(
        const std::string& notifcation_id) = 0;

    // Request to show the notification settings (|notification_id| is used
    // to identify the requesting browser context).
    virtual void ShowSettings(const std::string& notifcation_id) = 0;

    // Called when the notification body is clicked on.
    virtual void OnClicked(const std::string& notifcation_id) = 0;
  };

  explicit WebNotificationTray(internal::StatusAreaWidget* status_area_widget);
  virtual ~WebNotificationTray();

  // Called once to set the delegate.
  void SetDelegate(Delegate* delegate);

  // Add a new notification. |id| is a unique identifier, used to update or
  // remove notifications. |title| and |meesage| describe the notification text.
  // Use SetNotificationImage to set the icon image. If |extension_id| is
  // provided then 'Disable extension' will appear in a dropdown menu and the
  // id will be used to disable notifications from the extension. Otherwise if
  // |display_source| is provided, a menu item showing the source and allowing
  // notifications from that source to be disabled will be shown. All actual
  // disabling is handled by the Delegate.
  void AddNotification(const std::string& id,
                       const string16& title,
                       const string16& message,
                       const string16& display_source,
                       const std::string& extension_id);

  // Update an existing notification with id = old_id and set its id to new_id.
  void UpdateNotification(const std::string& old_id,
                          const std::string& new_id,
                          const string16& title,
                          const string16& message);

  // Remove an existing notification.
  void RemoveNotification(const std::string& id);

  // Set the notification image.
  void SetNotificationImage(const std::string& id,
                            const gfx::ImageSkia& image);

  // Set whether or not the popup notifications should be hidden.
  void SetHidePopupBubble(bool hide);

  // Updates tray visibility login status of the system changes.
  void UpdateAfterLoginStatusChange(user::LoginStatus login_status);

  // Returns true if the message center bubble is visible.
  bool IsMessageCenterBubbleVisible() const;

  // Returns true if the mouse is inside the notification bubble.
  bool IsMouseInNotificationBubble() const;

  // Overridden from TrayBackgroundView.
  virtual void SetShelfAlignment(ShelfAlignment alignment) OVERRIDE;
  virtual void AnchorUpdated() OVERRIDE;
  virtual string16 GetAccessibleNameForTray() OVERRIDE;
  virtual void HideBubbleWithView(
      const message_center::TrayBubbleView* bubble_view) OVERRIDE;
  virtual bool ClickedOutsideBubble() OVERRIDE;

  // Overridden from internal::ActionableView.
  virtual bool PerformAction(const ui::Event& event) OVERRIDE;

  // Overridden from message_center::TrayBubbleView::Delegate.
  virtual void BubbleViewDestroyed() OVERRIDE;
  virtual void OnMouseEnteredView() OVERRIDE;
  virtual void OnMouseExitedView() OVERRIDE;
  virtual string16 GetAccessibleNameForBubble() OVERRIDE;
  virtual gfx::Rect GetAnchorRect(views::Widget* anchor_widget,
                                  AnchorType anchor_type,
                                  AnchorAlignment anchor_alignment) OVERRIDE;
  virtual void HideBubble(
      const message_center::TrayBubbleView* bubble_view) OVERRIDE;

  // Overridden from message_center::WebNotificationList::Delegate.
  virtual void SendRemoveNotification(const std::string& id) OVERRIDE;
  virtual void SendRemoveAllNotifications() OVERRIDE;
  virtual void DisableNotificationByExtension(const std::string& id) OVERRIDE;
  virtual void DisableNotificationByUrl(const std::string& id) OVERRIDE;
  virtual void ShowNotificationSettings(const std::string& id) OVERRIDE;
  virtual void OnNotificationClicked(const std::string& id) OVERRIDE;
  virtual message_center::WebNotificationList* GetNotificationList() OVERRIDE;

  // Overridden from ButtonListener.
  virtual void ButtonPressed(views::Button* sender,
                             const ui::Event& event) OVERRIDE;

 private:
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest, WebNotifications);
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest, WebNotificationPopupBubble);
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest,
                           ManyMessageCenterNotifications);
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest, ManyPopupNotifications);

  // Shows or hides the message center bubble.
  void ToggleMessageCenterBubble();

  // Shows or updates the message center bubble and hides the popup bubble.
  void ShowMessageCenterBubble();

  // Hides the message center bubble if visible.
  void HideMessageCenterBubble();

  // Shows or updates the popup notification bubble if appropriate.
  void ShowPopupBubble();

  // Hides the notification bubble if visible.
  void HidePopupBubble();

  // Updates the tray icon and visibility.
  void UpdateTray();

  // As above but also updates any visible bubble.
  void UpdateTrayAndBubble();

  message_center::WebNotificationList* notification_list() {
    return notification_list_.get();
  }

  internal::WebNotificationBubbleWrapper* message_center_bubble() const {
    return message_center_bubble_.get();
  }

  internal::WebNotificationBubbleWrapper* popup_bubble() const {
    return popup_bubble_.get();
  }

  // Testing accessors.
  message_center::MessageCenterBubble* GetMessageCenterBubbleForTest();
  message_center::PopupBubble* GetPopupBubbleForTest();

  scoped_ptr<message_center::WebNotificationList> notification_list_;
  scoped_ptr<internal::WebNotificationBubbleWrapper> message_center_bubble_;
  scoped_ptr<internal::WebNotificationBubbleWrapper> popup_bubble_;
  views::ImageButton* button_;
  Delegate* delegate_;
  bool show_message_center_on_unlock_;

  DISALLOW_COPY_AND_ASSIGN(WebNotificationTray);
};

}  // namespace ash

#endif  // ASH_SYSTEM_WEB_NOTIFICATION_WEB_NOTIFICATION_TRAY_H_
