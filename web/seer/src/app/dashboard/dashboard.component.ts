import { Component, OnInit } from '@angular/core';
import { SeerConnectionService } from "app/services/seer-connection/seer-connection.service";
import { Observable } from "rxjs/Rx";

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {

  protected data: any[] = [];
  private seerObservable: Observable<any>;
  constructor(private seerConnection: SeerConnectionService) { }

  public ngOnInit() {
    this.seerObservable = this.seerConnection.connect('ws://localhost:9000');
    this.seerObservable.subscribe((message: any) => {
      console.log('received message from server: ', message);
      this.data.push(message);
    })

  }
}
